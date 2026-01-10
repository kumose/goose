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
#include <goose/planner/bound_result_modifier.h>

namespace goose {

class PhysicalStreamingLimit : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::STREAMING_LIMIT;

public:
	PhysicalStreamingLimit(PhysicalPlan &physical_plan, vector<LogicalType> types, BoundLimitNode limit_val_p,
	                       BoundLimitNode offset_val_p, idx_t estimated_cardinality, bool parallel);

	BoundLimitNode limit_val;
	BoundLimitNode offset_val;
	bool parallel;

public:
	// Operator interface
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	unique_ptr<GlobalOperatorState> GetGlobalOperatorState(ClientContext &context) const override;
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	OrderPreservationType OperatorOrder() const override;
	bool ParallelOperator() const override;
};

} // namespace goose
