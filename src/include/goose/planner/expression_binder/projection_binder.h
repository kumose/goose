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

class ColumnAliasBinder;

//! The Projection binder
class ProjectionBinder : public ExpressionBinder {
public:
	ProjectionBinder(Binder &binder, ClientContext &context, idx_t proj_index,
	                 vector<unique_ptr<Expression>> &proj_expressions, string clause);

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	                          bool root_expression = false) override;

	string UnsupportedAggregateMessage() override;

	BindResult BindColumnRef(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression);

private:
	idx_t proj_index;
	vector<unique_ptr<Expression>> &proj_expressions;
	string clause;
};

} // namespace goose
