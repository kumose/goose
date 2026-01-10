// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

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
