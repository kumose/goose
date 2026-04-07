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
#include <goose/common/vector.h>
#include <goose/parser/query_node.h>

namespace goose {

class UpdateSetInfo {
public:
	UpdateSetInfo();

public:
	unique_ptr<UpdateSetInfo> Copy() const;
	string ToString() const;

public:
	// The condition that needs to be met to perform the update
	unique_ptr<ParsedExpression> condition;
	// The columns to update
	vector<string> columns;
	// The set expressions to execute
	vector<unique_ptr<ParsedExpression>> expressions;

protected:
	UpdateSetInfo(const UpdateSetInfo &other);
};

class UpdateStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::UPDATE_STATEMENT;

public:
	UpdateStatement();

	unique_ptr<TableRef> table;
	unique_ptr<TableRef> from_table;
	//! keep track of optional returningList if statement contains a RETURNING keyword
	vector<unique_ptr<ParsedExpression>> returning_list;
	unique_ptr<UpdateSetInfo> set_info;
	//! CTEs
	CommonTableExpressionMap cte_map;

protected:
	UpdateStatement(const UpdateStatement &other);

public:
	string ToString() const override;
	unique_ptr<SQLStatement> Copy() const override;
};

} // namespace goose
