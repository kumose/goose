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
#include <goose/planner/column_binding_map.h>
#include <goose/planner/logical_operator.h>

namespace goose {

//! Helper class to rewrite correlated expressions within a single LogicalOperator
class RewriteCorrelatedExpressions : public LogicalOperatorVisitor {
public:
	RewriteCorrelatedExpressions(ColumnBinding base_binding, column_binding_map_t<idx_t> &correlated_map,
	                             idx_t lateral_depth, bool recursive_rewrite = false);

	void VisitOperator(LogicalOperator &op) override;

protected:
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	unique_ptr<Expression> VisitReplace(BoundSubqueryExpression &expr, unique_ptr<Expression> *expr_ptr) override;

private:
	ColumnBinding base_binding;
	column_binding_map_t<idx_t> &correlated_map;
	// To keep track of the number of dependent joins encountered
	idx_t lateral_depth;
	// This flag is used to determine if the rewrite should recursively update the bindings for all
	// bound columns ref in the plan, and update the depths to match the new source
	bool recursive_rewrite;
};

//! Helper class that rewrites COUNT aggregates into a CASE expression turning NULL into 0 after a LEFT OUTER JOIN
class RewriteCountAggregates : public LogicalOperatorVisitor {
public:
	explicit RewriteCountAggregates(column_binding_map_t<idx_t> &replacement_map);

	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;

	column_binding_map_t<idx_t> &replacement_map;
};

} // namespace goose
