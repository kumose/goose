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

#include <goose/execution/operator/join/physical_delim_join.h>

namespace goose {

//! PhysicalRightDelimJoin represents a join where the RHS will be duplicate eliminated and pushed into a
//! PhysicalColumnDataScan in the LHS.
class PhysicalRightDelimJoin : public PhysicalDelimJoin {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::RIGHT_DELIM_JOIN;

public:
	PhysicalRightDelimJoin(PhysicalPlan &physical_plan, PhysicalPlanGenerator &planner, vector<LogicalType> types,
	                       PhysicalOperator &original_join, PhysicalOperator &distinct,
	                       const vector<const_reference<PhysicalOperator>> &delim_scans, idx_t estimated_cardinality,
	                       optional_idx delim_idx);

public:
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	void PrepareFinalize(ClientContext &context, GlobalSinkState &sink_state) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
};

} // namespace goose
