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
