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

#include <goose/execution/physical_operator.h>
#include <goose/planner/expression.h>
#include <goose/common/string_map_set.h>
#include <goose/planner/tableref/bound_pivotref.h>

namespace goose {

//! PhysicalPivot implements the physical PIVOT operation
class PhysicalPivot : public PhysicalOperator {
public:
	PhysicalPivot(PhysicalPlan &physical_plan, vector<LogicalType> types, PhysicalOperator &child,
	              BoundPivotInfo bound_pivot);

	BoundPivotInfo bound_pivot;
	//! The map for pivot value -> column index
	string_map_t<idx_t> pivot_map;
	//! The empty aggregate values
	vector<Value> empty_aggregates;

public:
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	bool ParallelOperator() const override {
		return true;
	}
};

} // namespace goose
