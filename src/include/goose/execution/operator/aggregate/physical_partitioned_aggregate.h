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
#include <goose/planner/expression.h>
#include <goose/execution/operator/aggregate/grouped_aggregate_data.h>
#include <goose/parser/group_by_node.h>
#include <goose/execution/radix_partitioned_hashtable.h>
#include <goose/common/types-import.h>

namespace goose {

//! PhysicalPartitionedAggregate is an aggregate operator that can only perform aggregates on data that is partitioned
// by the grouping columns
class PhysicalPartitionedAggregate : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::PARTITIONED_AGGREGATE;

public:
	PhysicalPartitionedAggregate(PhysicalPlan &physical_plan, ClientContext &context, vector<LogicalType> types,
	                             vector<unique_ptr<Expression>> expressions, vector<unique_ptr<Expression>> groups,
	                             vector<column_t> partitions, idx_t estimated_cardinality);

	//! The partitions over which this is grouped
	vector<column_t> partitions;
	//! The groups over which the aggregate is partitioned - note that this is only
	vector<unique_ptr<Expression>> groups;
	//! The aggregates that have to be computed
	vector<unique_ptr<Expression>> aggregates;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;
	SinkNextBatchType NextBatch(ExecutionContext &context, OperatorSinkNextBatchInput &input) const override;

	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	InsertionOrderPreservingMap<string> ParamsToString() const override;

	OperatorPartitionInfo RequiredPartitionInfo() const override;
	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return true;
	}
};

} // namespace goose
