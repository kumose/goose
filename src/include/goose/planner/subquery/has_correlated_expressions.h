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

#include <goose/planner/binder.h>
#include <goose/planner/logical_operator.h>

namespace goose {

//! Helper class to recursively detect correlated expressions inside a single LogicalOperator
class HasCorrelatedExpressions : public LogicalOperatorVisitor {
public:
	explicit HasCorrelatedExpressions(const CorrelatedColumns &correlated, bool lateral = false,
	                                  idx_t lateral_depth = 0);

	void VisitOperator(LogicalOperator &op) override;

	bool has_correlated_expressions;
	bool lateral;

protected:
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	unique_ptr<Expression> VisitReplace(BoundSubqueryExpression &expr, unique_ptr<Expression> *expr_ptr) override;

	const CorrelatedColumns &correlated_columns;
	// Tracks number of nested laterals
	idx_t lateral_depth;
};

} // namespace goose
