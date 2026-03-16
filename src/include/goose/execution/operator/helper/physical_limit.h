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
#include <goose/planner/bound_result_modifier.h>

namespace goose {

//! PhyisicalLimit represents the LIMIT operator
class PhysicalLimit : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::LIMIT;

	static constexpr const idx_t MAX_LIMIT_VALUE = 1ULL << 62ULL;

public:
	PhysicalLimit(PhysicalPlan &physical_plan, vector<LogicalType> types, BoundLimitNode limit_val,
	              BoundLimitNode offset_val, idx_t estimated_cardinality);

	BoundLimitNode limit_val;
	BoundLimitNode offset_val;

public:
	bool SinkOrderDependent() const override {
		return true;
	}

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink Interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return true;
	}

	OperatorPartitionInfo RequiredPartitionInfo() const override {
		return OperatorPartitionInfo::BatchIndex();
	}

public:
	static void SetInitialLimits(const BoundLimitNode &limit_val, const BoundLimitNode &offset_val, optional_idx &limit,
	                             optional_idx &offset);
	static bool ComputeOffset(ExecutionContext &context, DataChunk &input, optional_idx &limit, optional_idx &offset,
	                          idx_t current_offset, idx_t &max_element, const BoundLimitNode &limit_val,
	                          const BoundLimitNode &offset_val);
	static bool HandleOffset(DataChunk &input, idx_t &current_offset, idx_t offset, idx_t limit);
	static Value GetDelimiter(ExecutionContext &context, DataChunk &input, const Expression &expr);
};

} // namespace goose
