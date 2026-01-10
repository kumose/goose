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

namespace goose {

//! PhysicalUnnest implements the physical UNNEST operation
class PhysicalUnnest : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::UNNEST;

public:
	PhysicalUnnest(PhysicalPlan &physical_plan, vector<LogicalType> types, vector<unique_ptr<Expression>> select_list,
	               idx_t estimated_cardinality, PhysicalOperatorType type = PhysicalOperatorType::UNNEST);

	//! The projection list of the UNNEST
	//! E.g. SELECT 1, UNNEST([1]), UNNEST([2, 3]); has two UNNESTs in its select_list
	vector<unique_ptr<Expression>> select_list;

public:
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	bool ParallelOperator() const override {
		return true;
	}

public:
	static unique_ptr<OperatorState> GetState(ExecutionContext &context,
	                                          const vector<unique_ptr<Expression>> &select_list);
	//! Executes the UNNEST operator internally and emits a chunk of unnested data. If include_input is set, then
	//! the resulting chunk also contains vectors for all non-UNNEST columns in the projection. If include_input is
	//! not set, then the UNNEST behaves as a table function and only emits the unnested data.
	static OperatorResultType ExecuteInternal(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                                          OperatorState &state, const vector<unique_ptr<Expression>> &select_list,
	                                          bool include_input = true);
};

} // namespace goose
