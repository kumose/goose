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
