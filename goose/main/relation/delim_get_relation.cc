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


#include <goose/main/relation/delim_get_relation.h>
#include <goose/main/client_context.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/expression/star_expression.h>
#include <goose/parser/tableref/delimgetref.h>

namespace goose {
    DelimGetRelation::DelimGetRelation(const shared_ptr<ClientContext> &context, vector<LogicalType> chunk_types_p)
        : Relation(context, RelationType::DELIM_GET_RELATION), chunk_types(std::move(chunk_types_p)) {
        TryBindRelation(columns);
    }

    unique_ptr<QueryNode> DelimGetRelation::GetQueryNode() {
        auto result = make_uniq<SelectNode>();
        result->select_list.push_back(make_uniq<StarExpression>());
        result->from_table = GetTableRef();
        return std::move(result);
    }

    unique_ptr<TableRef> DelimGetRelation::GetTableRef() {
        auto delim_get_ref = make_uniq<DelimGetRef>(chunk_types);
        return std::move(delim_get_ref);
    }

    const vector<ColumnDefinition> &DelimGetRelation::Columns() {
        return this->columns;
    }

    string DelimGetRelation::ToString(idx_t depth) {
        string str = RenderWhitespace(depth);
        str += "Delimiter Get [";
        for (idx_t i = 0; i < chunk_types.size(); i++) {
            str += chunk_types[i].ToString();
            if (i + 1 < chunk_types.size()) {
                str += ", ";
            }
        }
        str += "]";

        return str;
    }
} // namespace goose
