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
