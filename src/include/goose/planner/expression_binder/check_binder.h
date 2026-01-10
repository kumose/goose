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

#include <goose/parser/column_definition.h>
#include <goose/planner/expression_binder.h>
#include <goose/common/index_map.h>
#include <goose/parser/column_list.h>

namespace goose {
//! The CHECK binder is responsible for binding an expression within a CHECK constraint
class CheckBinder : public ExpressionBinder {
public:
	CheckBinder(Binder &binder, ClientContext &context, string table, const ColumnList &columns,
	            physical_index_set_t &bound_columns);

	string table;
	const ColumnList &columns;
	physical_index_set_t &bound_columns;

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	                          bool root_expression = false) override;

	BindResult BindCheckColumn(ColumnRefExpression &expr);

	string UnsupportedAggregateMessage() override;
};

} // namespace goose
