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
