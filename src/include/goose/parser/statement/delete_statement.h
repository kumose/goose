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

#pragma once

#include <goose/parser/parsed_expression.h>
#include <goose/parser/sql_statement.h>
#include <goose/parser/tableref.h>
#include <goose/parser/query_node.h>

namespace goose {

class DeleteStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::DELETE_STATEMENT;

public:
	DeleteStatement();

	unique_ptr<ParsedExpression> condition;
	unique_ptr<TableRef> table;
	vector<unique_ptr<TableRef>> using_clauses;
	vector<unique_ptr<ParsedExpression>> returning_list;
	//! CTEs
	CommonTableExpressionMap cte_map;

protected:
	DeleteStatement(const DeleteStatement &other);

public:
	string ToString() const override;
	unique_ptr<SQLStatement> Copy() const override;
};

} // namespace goose
