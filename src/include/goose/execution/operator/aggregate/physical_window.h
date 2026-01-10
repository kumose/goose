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

namespace goose {

//! PhysicalWindow implements window functions
//! It assumes that all functions have a common partitioning and ordering
class PhysicalWindow : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::WINDOW;

public:
	PhysicalWindow(PhysicalPlan &physical_plan, vector<LogicalType> types, vector<unique_ptr<Expression>> select_list,
	               idx_t estimated_cardinality, PhysicalOperatorType type = PhysicalOperatorType::WINDOW);

	//! The projection list of the WINDOW statement (may contain aggregates)
	vector<unique_ptr<Expression>> select_list;
	//! The window expression with the order clause
	idx_t order_idx;
	//! Whether or not the window is order dependent (only true if ANY window function contains neither an order nor a
	//! partition clause)
	bool is_order_dependent;

public:
	// Source interface
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;
	OperatorPartitionData GetPartitionData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
	                                       LocalSourceState &lstate,
	                                       const OperatorPartitionInfo &partition_info) const override;

	bool IsSource() const override {
		return true;
	}
	bool ParallelSource() const override {
		return true;
	}

	bool SupportsPartitioning(const OperatorPartitionInfo &partition_info) const override;
	OrderPreservationType SourceOrder() const override;

	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate_p) const override;

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;
	ProgressData GetSinkProgress(ClientContext &context, GlobalSinkState &gstate,
	                             const ProgressData source_progress) const override;

	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return !is_order_dependent;
	}

	bool SinkOrderDependent() const override {
		return is_order_dependent;
	}

public:
	idx_t MaxThreads(ClientContext &context);

	InsertionOrderPreservingMap<string> ParamsToString() const override;
};

} // namespace goose
