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

#include <goose/execution/operator/join/physical_comparison_join.h>

namespace goose {

//! PhysicalNestedLoopJoin represents a nested loop join between two tables
class PhysicalNestedLoopJoin : public PhysicalComparisonJoin {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::NESTED_LOOP_JOIN;

public:
	PhysicalNestedLoopJoin(PhysicalPlan &physical_plan, LogicalComparisonJoin &op, PhysicalOperator &left,
	                       PhysicalOperator &right, vector<JoinCondition> cond, JoinType join_type,
	                       idx_t estimated_cardinality, unique_ptr<JoinFilterPushdownInfo> pushdown_info);
	PhysicalNestedLoopJoin(PhysicalPlan &physical_plan, LogicalComparisonJoin &op, PhysicalOperator &left,
	                       PhysicalOperator &right, vector<JoinCondition> cond, JoinType join_type,
	                       idx_t estimated_cardinality);

	// Predicate (join conditions that don't reference both sides)
	unique_ptr<Expression> predicate;

public:
	// Operator Interface
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;

	bool ParallelOperator() const override {
		return true;
	}

protected:
	// CachingOperator Interface
	OperatorResultType ExecuteInternal(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                                   GlobalOperatorState &gstate, OperatorState &state) const override;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return PropagatesBuildSide(join_type);
	}
	bool ParallelSource() const override {
		return true;
	}

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

	static bool IsSupported(const vector<JoinCondition> &conditions, JoinType join_type);

public:
	//! Returns a list of the types of the join conditions
	vector<LogicalType> GetJoinTypes() const;

private:
	// resolve joins that output max N elements (SEMI, ANTI, MARK)
	void ResolveSimpleJoin(ExecutionContext &context, DataChunk &input, DataChunk &chunk, OperatorState &state) const;
	// resolve joins that can potentially output N*M elements (INNER, LEFT, FULL)
	OperatorResultType ResolveComplexJoin(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                                      OperatorState &state) const;
};

} // namespace goose
