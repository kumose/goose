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
