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

#include <goose/execution/operator/join/physical_range_join.h>
#include <goose/planner/bound_result_modifier.h>

namespace goose {

//! PhysicalIEJoin represents a two inequality range join between
//! two tables
class PhysicalIEJoin : public PhysicalRangeJoin {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::IE_JOIN;

public:
	PhysicalIEJoin(PhysicalPlan &physical_plan, LogicalComparisonJoin &op, PhysicalOperator &left,
	               PhysicalOperator &right, vector<JoinCondition> cond, JoinType join_type, idx_t estimated_cardinality,
	               unique_ptr<JoinFilterPushdownInfo> pushdown_info);
	PhysicalIEJoin(PhysicalPlan &physical_plan, LogicalComparisonJoin &op, PhysicalOperator &left,
	               PhysicalOperator &right, vector<JoinCondition> cond, JoinType join_type,
	               idx_t estimated_cardinality);

	vector<LogicalType> join_key_types;
	vector<BoundOrderByNode> lhs_orders;
	vector<BoundOrderByNode> rhs_orders;

public:
	// CachingOperator Interface
	OperatorResultType ExecuteInternal(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                                   GlobalOperatorState &gstate, OperatorState &state) const override;

public:
	// Source interface
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
	bool ParallelSource() const override {
		return true;
	}

	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate_p) const override;

public:
	// Sink Interface
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;
};

} // namespace goose
