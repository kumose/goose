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

#include <goose/parser/parser.h>
#include <goose/main/client_context.h>
#include <goose/function/table/range.h>
#include <goose/function/function_set.h>
#include <goose/parser/tableref/subqueryref.h>
#include <goose/parser/statement/multi_statement.h>
#include <goose/parser/statement/select_statement.h>

namespace goose {
    static unique_ptr<SubqueryRef> ParseSubquery(const string &query, const ParserOptions &options,
                                                 const string &err_msg) {
        Parser parser(options);
        parser.ParseQuery(query);
        if (parser.statements.size() != 1) {
            throw ParserException(err_msg);
        }

        auto &stmt = parser.statements[0];

        if (stmt->type == StatementType::SELECT_STATEMENT) {
            // Regular SELECT statement
            auto select_stmt = unique_ptr_cast<SQLStatement, SelectStatement>(std::move(stmt));
            return goose::make_uniq<SubqueryRef>(std::move(select_stmt));
        } else if (stmt->type == StatementType::MULTI_STATEMENT) {
            // MultiStatement (e.g., from PIVOT statements that create enum types)
            throw ParserException(
                "PIVOT statements without explicit IN clauses are not supported in query() function. "
                "Please specify the pivot values explicitly, e.g.: PIVOT ... ON col IN (val1, val2, ...)");
        } else {
            throw ParserException(err_msg);
        }
    }

    static string UnionTablesQuery(TableFunctionBindInput &input) {
        for (auto &input_val: input.inputs) {
            if (input_val.IsNull()) {
                throw BinderException("Cannot use NULL as function argument");
            }
        }
        string result;
        string by_name = (input.inputs.size() == 2 &&
                          (input.inputs[1].type().id() == LogicalTypeId::BOOLEAN && input.inputs[1].GetValue<bool>()))
                             ? "BY NAME "
                             : ""; // 'by_name' variable defaults to false
        if (input.inputs[0].type().id() == LogicalTypeId::VARCHAR) {
            auto from_path = input.inputs[0].ToString();
            auto qualified_name = QualifiedName::Parse(from_path);
            result += "FROM " + qualified_name.ToString();
        } else if (input.inputs[0].type() == LogicalType::LIST(LogicalType::VARCHAR)) {
            string union_all_clause = " UNION ALL " + by_name + "FROM ";
            const auto &children = ListValue::GetChildren(input.inputs[0]);

            if (children.empty()) {
                throw InvalidInputException("Input list is empty");
            }
            auto qualified_name = QualifiedName::Parse(children[0].ToString());
            result += "FROM " + qualified_name.ToString();
            for (size_t i = 1; i < children.size(); ++i) {
                auto child = children[i].ToString();
                auto qualified_name = QualifiedName::Parse(child);
                result += union_all_clause + qualified_name.ToString();
            }
        } else {
            throw InvalidInputException("Expected a table or a list with tables as input");
        }
        return result;
    }

    static unique_ptr<TableRef> QueryBindReplace(ClientContext &context, TableFunctionBindInput &input) {
        auto query = input.inputs[0].ToString();
        auto subquery_ref = ParseSubquery(query, context.GetParserOptions(), "Expected a single SELECT statement");
        return subquery_ref;
    }

    static unique_ptr<TableRef> TableBindReplace(ClientContext &context, TableFunctionBindInput &input) {
        auto query = UnionTablesQuery(input);
        auto subquery_ref =
                ParseSubquery(query, context.GetParserOptions(), "Expected a table or a list with tables as input");
        return subquery_ref;
    }

    void QueryTableFunction::RegisterFunction(BuiltinFunctions &set) {
        TableFunction query("query", {LogicalType::VARCHAR}, nullptr, nullptr);
        query.bind_replace = QueryBindReplace;
        set.AddFunction(query);

        TableFunctionSet query_table("query_table");
        TableFunction query_table_function({LogicalType::VARCHAR}, nullptr, nullptr);
        query_table_function.bind_replace = TableBindReplace;
        query_table.AddFunction(query_table_function);

        query_table_function.arguments = {LogicalType::LIST(LogicalType::VARCHAR)};
        query_table.AddFunction(query_table_function);
        // add by_name option
        query_table_function.arguments.emplace_back(LogicalType::BOOLEAN);
        query_table.AddFunction(query_table_function);
        set.AddFunction(query_table);
    }
} // namespace goose
