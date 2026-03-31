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
//===----------------------------------------------------------------------===//
//                         Goose
//
// goose/function/table_macro_function.hpp
//
//
//===----------------------------------------------------------------------===//
//! The SelectStatement of the view
#include <goose/function/table_macro_function.h>

#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/query_node.h>

namespace goose {
    TableMacroFunction::TableMacroFunction(unique_ptr<QueryNode> query_node)
        : MacroFunction(MacroType::TABLE_MACRO), query_node(std::move(query_node)) {
    }

    TableMacroFunction::TableMacroFunction(void) : MacroFunction(MacroType::TABLE_MACRO) {
    }

    unique_ptr<MacroFunction> TableMacroFunction::Copy() const {
        auto result = make_uniq<TableMacroFunction>();
        result->query_node = query_node->Copy();
        this->CopyProperties(*result);
        return std::move(result);
    }

    string TableMacroFunction::ToSQL() const {
        return MacroFunction::ToSQL() + StringUtil::Format("TABLE (%s)", query_node->ToString());
    }
} // namespace goose
