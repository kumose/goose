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

#include <goose/common/value_operations/value_operations.h>
#include <goose/execution/join_hashtable.h>
#include <goose/execution/operator/join/perfect_hash_join_executor.h>
#include <goose/execution/operator/join/physical_comparison_join.h>
#include <goose/execution/physical_operator.h>
#include <goose/planner/operator/logical_join.h>

namespace goose {

//! PhysicalHashJoin represents a hash loop join between two tables
class PhysicalHashJoin : public PhysicalComparisonJoin {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::HASH_JOIN;

	struct JoinProjectionColumns {
		vector<idx_t> col_idxs;
		vector<LogicalType> col_types;
	};

public:
	PhysicalHashJoin(PhysicalPlan &physical_plan, LogicalOperator &op, PhysicalOperator &left, PhysicalOperator &right,
	                 vector<JoinCondition> cond, JoinType join_type, const vector<idx_t> &left_projection_map,
	                 const vector<idx_t> &right_projection_map, vector<LogicalType> delim_types,
	                 idx_t estimated_cardinality, unique_ptr<JoinFilterPushdownInfo> pushdown_info);
	PhysicalHashJoin(PhysicalPlan &physical_plan, LogicalOperator &op, PhysicalOperator &left, PhysicalOperator &right,
	                 vector<JoinCondition> cond, JoinType join_type, idx_t estimated_cardinality);

	//! Initialize HT for this operator
	unique_ptr<JoinHashTable> InitializeHashTable(ClientContext &context) const;

	//! The types of the join keys
	vector<LogicalType> condition_types;

	//! The indices/types of the payload columns
	JoinProjectionColumns payload_columns;
	//! The indices/types of the lhs columns that need to be output
	JoinProjectionColumns lhs_output_columns;
	//! The indices/types of the rhs columns that need to be output
	JoinProjectionColumns rhs_output_columns;

	//! Duplicate eliminated types; only used for delim_joins (i.e. correlated subqueries)
	vector<LogicalType> delim_types;

	//! Join Keys statistics (optional)
	vector<unique_ptr<BaseStatistics>> join_stats;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

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

	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                 GlobalSourceState &gstate) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const override;

	//! Becomes a source when it is an external join
	bool IsSource() const override {
		return true;
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
	void PrepareFinalize(ClientContext &context, GlobalSinkState &global_state) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;

	bool IsSink() const override {
		return true;
	}
	bool ParallelSink() const override {
		return true;
	}
};

} // namespace goose
