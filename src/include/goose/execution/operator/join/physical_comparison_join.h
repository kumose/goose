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

#include <goose/execution/expression_executor.h>
#include <goose/execution/operator/join/physical_join.h>
#include <goose/execution/operator/join/join_filter_pushdown.h>

namespace goose {
class ColumnDataCollection;
struct ColumnDataScanState;
class LogicalGet;

//! PhysicalJoin represents the base class of the join operators
class PhysicalComparisonJoin : public PhysicalJoin {
public:
	PhysicalComparisonJoin(PhysicalPlan &physical_plan, LogicalOperator &op, PhysicalOperatorType type,
	                       vector<JoinCondition> cond, JoinType join_type, idx_t estimated_cardinality);

	vector<JoinCondition> conditions;
	//! Scans where we should push generated filters into (if any)
	unique_ptr<JoinFilterPushdownInfo> filter_pushdown;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	//! Re-order join conditions so that equality predicates are first, followed by other predicates
	static void ReorderConditions(vector<JoinCondition> &conditions);

	//! Construct the join result of a join with an empty RHS
	static void ConstructEmptyJoinResult(JoinType type, bool has_null, DataChunk &input, DataChunk &result);
	//! Construct the remainder of a Full Outer Join based on which tuples in the RHS found no match
	static void ConstructFullOuterJoinResult(bool *found_match, ColumnDataCollection &input, DataChunk &result,
	                                         ColumnDataScanState &scan_state);
};

} // namespace goose
