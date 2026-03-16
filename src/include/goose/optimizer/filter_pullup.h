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

#include <goose/common/types-import.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/operator/logical_filter.h>
#include <goose/common/vector.h>

namespace goose {

class FilterPullup {
public:
	explicit FilterPullup(bool pullup = false, bool add_column = false)
	    : can_pullup(pullup), can_add_column(add_column) {
	}

	//! Perform filter pullup
	unique_ptr<LogicalOperator> Rewrite(unique_ptr<LogicalOperator> op);

private:
	vector<unique_ptr<Expression>> filters_expr_pullup;

	// only pull up filters when there is a fork
	bool can_pullup = false;

	// identify case the branch is a set operation (INTERSECT or EXCEPT)
	bool can_add_column = false;

private:
	// Generate logical filters pulled up
	unique_ptr<LogicalOperator> GeneratePullupFilter(unique_ptr<LogicalOperator> child,
	                                                 vector<unique_ptr<Expression>> &expressions);

	//! Pull up a LogicalFilter op
	unique_ptr<LogicalOperator> PullupFilter(unique_ptr<LogicalOperator> op);
	//! Pull up filter in a LogicalProjection op
	unique_ptr<LogicalOperator> PullupProjection(unique_ptr<LogicalOperator> op);
	//! Pull up filter in a LogicalCrossProduct op
	unique_ptr<LogicalOperator> PullupCrossProduct(unique_ptr<LogicalOperator> op);
	//! Pullup a filter in a LogicalJoin
	unique_ptr<LogicalOperator> PullupJoin(unique_ptr<LogicalOperator> op);
	//! Pullup filter in a left join
	unique_ptr<LogicalOperator> PullupFromLeft(unique_ptr<LogicalOperator> op);
	//! Pullup filter in an inner join
	unique_ptr<LogicalOperator> PullupInnerJoin(unique_ptr<LogicalOperator> op);
	//! Pullup filter through a distinct
	unique_ptr<LogicalOperator> PullupDistinct(unique_ptr<LogicalOperator> op);
	//! Pullup filter in LogicalIntersect or LogicalExcept op
	unique_ptr<LogicalOperator> PullupSetOperation(unique_ptr<LogicalOperator> op);
	//! Pullup filter in both sides of a join
	unique_ptr<LogicalOperator> PullupBothSide(unique_ptr<LogicalOperator> op);

	//! Finish pull up at this operator
	unique_ptr<LogicalOperator> FinishPullup(unique_ptr<LogicalOperator> op);
	//! special treatment for SetOperations and projections
	void ProjectSetOperation(LogicalProjection &proj);

}; // end FilterPullup

} // namespace goose
