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
