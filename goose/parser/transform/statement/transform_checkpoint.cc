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


#include <goose/parser/transformer.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/parser/statement/call_statement.h>
#include <goose/parser/expression/constant_expression.h>

namespace goose {
    unique_ptr<SQLStatement> Transformer::TransformCheckpoint(cantor::PGCheckPointStmt &stmt) {
        vector<unique_ptr<ParsedExpression> > children;
        // transform into "CALL checkpoint()" or "CALL force_checkpoint()"
        auto checkpoint_name = stmt.force ? "force_checkpoint" : "checkpoint";
        auto result = make_uniq<CallStatement>();
        auto function = make_uniq<FunctionExpression>(checkpoint_name, std::move(children));
        function->catalog = SYSTEM_CATALOG;
        function->schema = DEFAULT_SCHEMA;
        if (stmt.name) {
            function->children.push_back(make_uniq<ConstantExpression>(Value(stmt.name)));
        }
        result->function = std::move(function);
        return std::move(result);
    }
} // namespace goose
