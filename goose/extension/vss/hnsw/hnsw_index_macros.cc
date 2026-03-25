// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <goose/function/table_macro_function.h>
#include <goose/main/extension/extension_loader.h>
#include <goose/extension/vss/hnsw/hnsw.h>
#include <goose/parser/parser.h>
#include <goose/parser/parsed_data/create_macro_info.h>

namespace goose {
    static constexpr auto VSS_JOIN_MACRO = R"(
SELECT
	score,
	left_tbl,
	right_tbl,
FROM
	(SELECT * FROM query_table(left_table::VARCHAR)) as left_tbl,
	(
		SELECT
			struct_pack(*columns([x for x in (matches.*) if x != 'score'])) as right_tbl,
			matches.score as score
		FROM (
			SELECT (
				unnest(
					CASE WHEN metric = 'l2sq' OR metric = 'l2'
						THEN min_by(tbl, tbl.score, k)
						ELSE max_by(tbl, tbl.score, k)
					END,
					max_depth := 2
				)
			) as result
			FROM (
				SELECT
				*,
				CASE
					WHEN metric = 'l2sq' OR metric = 'l2'
					THEN array_distance(left_col, right_col)
					WHEN metric = 'cosine' OR metric = 'cos'
					THEN array_cosine_similarity(left_col, right_col)
					WHEN metric = 'ip'
					THEN array_inner_product(left_col, right_col)
					ELSE error('Unknown metric')
				END as score,
				FROM query_table(right_table::VARCHAR)
			) as tbl
		) as matches
	)
)";

    static constexpr auto VSS_MATCH_MACRO = R"(
SELECT
	right_tbl as matches,
FROM
	(
		SELECT (
			CASE WHEN metric = 'l2sq' OR metric = 'l2'
				THEN min_by({'score': score, 'row': t}, score, k)
				ELSE max_by({'score': score, 'row': t}, score, k)
			END
		) as right_tbl
		FROM (
			SELECT
			CASE
				WHEN metric = 'l2sq' OR metric = 'l2'
				THEN array_distance(left_col, right_col)
				WHEN metric = 'cosine' OR metric = 'cos'
				THEN array_cosine_similarity(left_col, right_col)
				WHEN metric = 'ip'
				THEN array_inner_product(left_col, right_col)
				ELSE error('Unknown metric')
			END as score,
			tbl as t,
			FROM (SELECT * FROM query_table(right_table::VARCHAR)) as tbl
		)
	)
)";

    //-------------------------------------------------------------------------
    // Register
    //-------------------------------------------------------------------------
    static void RegisterTableMacro(ExtensionLoader &loader, const string &name, const string &query,
                                   const vector<string> &params, const child_list_t<Value> &named_params) {
        Parser parser;
        parser.ParseQuery(query);
        const auto &stmt = parser.statements.back();
        auto &node = stmt->Cast<SelectStatement>().node;

        auto func = make_uniq<TableMacroFunction>(std::move(node));
        for (auto &param: params) {
            func->parameters.push_back(make_uniq<ColumnRefExpression>(param));
        }

        for (auto &param: named_params) {
            func->parameters.push_back(make_uniq<ColumnRefExpression>(param.first));
            func->default_parameters[param.first] = make_uniq<ConstantExpression>(param.second);
        }

        CreateMacroInfo info(CatalogType::TABLE_MACRO_ENTRY);
        info.schema = DEFAULT_SCHEMA;
        info.name = name;
        info.temporary = true;
        info.internal = true;
        info.macros.push_back(std::move(func));

        loader.RegisterFunction(info);
    }

    void HNSWModule::RegisterMacros(ExtensionLoader &loader) {
        RegisterTableMacro(loader, "vss_join", VSS_JOIN_MACRO,
                           {"left_table", "right_table", "left_col", "right_col", "k"},
                           {{"metric", Value("l2sq")}});

        RegisterTableMacro(loader, "vss_match", VSS_MATCH_MACRO, {"right_table", "left_col", "right_col", "k"},
                           {{"metric", Value("l2sq")}});
    }
} // namespace goose
