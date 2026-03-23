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
#include <goose/parallel/pipeline.h>
#include <goose/planner/bound_query_node.h>

namespace goose {

class OrderGlobalSinkState;

//! PhysicalOrder sorts the input data
class PhysicalOrder : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::ORDER_BY;

public:
	PhysicalOrder(PhysicalPlan &physical_plan, vector<LogicalType> types, vector<BoundOrderByNode> orders,
	              vector<idx_t> projections, idx_t estimated_cardinality, bool is_index_sort = false);

	vector<BoundOrderByNode> orders;
	vector<idx_t> projections;
	bool is_index_sort;

public:
	//===--------------------------------------------------------------------===//
	// Source Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;
	OperatorPartitionData GetPartitionData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
	                                       LocalSourceState &lstate,
	                                       const OperatorPartitionInfo &partition_info) const override;
	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const override;

	bool IsSource() const override {
		return true;
	}

	bool ParallelSource() const override {
		return true;
	}

	bool SupportsPartitioning(const OperatorPartitionInfo &partition_info) const override {
		if (partition_info.RequiresPartitionColumns()) {
			return false;
		}
		return true;
	}

	OrderPreservationType SourceOrder() const override {
		return OrderPreservationType::FIXED_ORDER;
	}

public:
	//===--------------------------------------------------------------------===//
	// Sink Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;
	ProgressData GetSinkProgress(ClientContext &context, GlobalSinkState &gstate,
	                             const ProgressData source_progress) const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}
	bool SinkOrderDependent() const override {
		return false;
	}

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;
};

} // namespace goose
