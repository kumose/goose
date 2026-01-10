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

#include <goose/planner/expression_binder.h>

namespace goose {

//! The table function binder can bind standard table function parameters (i.e., non-table-in-out functions)
class TableFunctionBinder : public ExpressionBinder {
public:
	TableFunctionBinder(Binder &binder, ClientContext &context, string table_function_name = string(),
	                    string clause = "Table function");

protected:
	BindResult BindLambdaReference(LambdaRefExpression &expr, idx_t depth);
	BindResult BindColumnReference(unique_ptr<ParsedExpression> &expr, idx_t depth, bool root_expression);
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr, idx_t depth, bool root_expression = false) override;

	string UnsupportedAggregateMessage() override;

private:
	string table_function_name;
	string clause;
};

} // namespace goose
