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
#include <goose/planner/binder.h>

namespace goose {

class ColumnAliasBinder;

//! The LATERAL binder is responsible for binding an expression within a LATERAL join
class LateralBinder : public ExpressionBinder {
public:
	LateralBinder(Binder &binder, ClientContext &context);

	bool HasCorrelatedColumns() const {
		return !correlated_columns.empty();
	}

	static void ReduceExpressionDepth(LogicalOperator &op, const CorrelatedColumns &info);

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	                          bool root_expression = false) override;

	string UnsupportedAggregateMessage() override;

private:
	BindResult BindColumnRef(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression);
	void ExtractCorrelatedColumns(Expression &expr);

private:
	CorrelatedColumns correlated_columns;
};

} // namespace goose
