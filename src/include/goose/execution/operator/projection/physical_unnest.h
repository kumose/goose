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
