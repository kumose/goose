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

#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/physical_operator.h>
#include <goose/planner/expression/bound_aggregate_expression.h>

namespace goose {

class RecursiveCTEState;

class PhysicalRecursiveCTE : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::RECURSIVE_CTE;

public:
	PhysicalRecursiveCTE(PhysicalPlan &physical_plan, string ctename, idx_t table_index, vector<LogicalType> types,
	                     bool union_all, PhysicalOperator &top, PhysicalOperator &bottom, idx_t estimated_cardinality);
	~PhysicalRecursiveCTE() override;

	string ctename;
	idx_t table_index;
	// Flag if recurring table is referenced, if not we do not copy ht into ColumnDataCollection
	bool ref_recurring;
	bool union_all;
	shared_ptr<ColumnDataCollection> working_table;
	shared_ptr<MetaPipeline> recursive_meta_pipeline;

	//===--------------------------------------------------------------------===//
	// Additionally required for using-key recursive CTE to normal CTE.
	//===--------------------------------------------------------------------===//
	bool using_key = false;
	// Contains the result of the key variant
	shared_ptr<ColumnDataCollection> recurring_table;
	// Contains the types of the payload and key columns.
	vector<LogicalType> payload_types, distinct_types;
	// Contains the payload and key indices
	vector<idx_t> payload_idx, distinct_idx;
	// Contains the aggregates for the payload
	vector<unique_ptr<BoundAggregateExpression>> payload_aggregates;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;

	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return true;
	}

	InsertionOrderPreservingMap<string> ParamsToString() const override;

public:
	void BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) override;

	vector<const_reference<PhysicalOperator>> GetSources() const override;

private:
	//! Probe Hash Table and eliminate duplicate rows
	idx_t ProbeHT(DataChunk &chunk, RecursiveCTEState &state) const;

	void ExecuteRecursivePipelines(ExecutionContext &context) const;
};

} // namespace goose
