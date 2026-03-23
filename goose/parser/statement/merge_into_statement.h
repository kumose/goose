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
#include <goose/common/enums/merge_action_type.h>
#include <goose/parser/statement/insert_statement.h>

namespace goose {

class MergeIntoAction {
public:
	//! The merge action type
	MergeActionType action_type;
	//! Condition - or NULL if this should always be performed for the given action
	unique_ptr<ParsedExpression> condition;
	//! The SET information (if action_type == MERGE_UPDATE)
	unique_ptr<UpdateSetInfo> update_info;
	//! Column names to insert into (if action_type == MERGE_INSERT)
	vector<string> insert_columns;
	//! Set of expressions for INSERT
	vector<unique_ptr<ParsedExpression>> expressions;
	//! INSERT BY POSITION or INSERT BY NAME
	InsertColumnOrder column_order = InsertColumnOrder::INSERT_BY_POSITION;
	//! Whether or not this is a INSERT DEFAULT VALUES
	bool default_values = false;

	string ToString() const;
	unique_ptr<MergeIntoAction> Copy() const;
};

class MergeIntoStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::MERGE_INTO_STATEMENT;

public:
	MergeIntoStatement();

	unique_ptr<TableRef> target;
	unique_ptr<TableRef> source;
	unique_ptr<ParsedExpression> join_condition;
	vector<string> using_columns;

	map<MergeActionCondition, vector<unique_ptr<MergeIntoAction>>> actions;

	//! keep track of optional returningList if statement contains a RETURNING keyword
	vector<unique_ptr<ParsedExpression>> returning_list;

	//! CTEs
	CommonTableExpressionMap cte_map;

protected:
	MergeIntoStatement(const MergeIntoStatement &other);

public:
	string ToString() const override;
	unique_ptr<SQLStatement> Copy() const override;

	static string ActionConditionToString(MergeActionCondition condition);
};

} // namespace goose
