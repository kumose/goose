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
#include <goose/execution/base_aggregate_hashtable.h>

namespace goose {
class ClientContext;
class PerfectAggregateHashTable;

//! PhysicalPerfectHashAggregate performs a group-by and aggregation using a perfect hash table
class PhysicalPerfectHashAggregate : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::PERFECT_HASH_GROUP_BY;

public:
	PhysicalPerfectHashAggregate(PhysicalPlan &physical_plan, ClientContext &context, vector<LogicalType> types,
	                             vector<unique_ptr<Expression>> aggregates, vector<unique_ptr<Expression>> groups,
	                             const vector<unique_ptr<BaseStatistics>> &group_stats, vector<idx_t> required_bits,
	                             idx_t estimated_cardinality);

	//! The groups
	vector<unique_ptr<Expression>> groups;
	//! The aggregates that have to be computed
	vector<unique_ptr<Expression>> aggregates;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}
	OrderPreservationType SourceOrder() const override {
		return OrderPreservationType::NO_ORDER;
	}

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;

	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	InsertionOrderPreservingMap<string> ParamsToString() const override;

	//! Create a perfect aggregate hash table for this node
	unique_ptr<PerfectAggregateHashTable> CreateHT(Allocator &allocator, ClientContext &context) const;

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
	//! The group types
	vector<LogicalType> group_types;
	//! The payload types
	vector<LogicalType> payload_types;
	//! The aggregates to be computed
	vector<AggregateObject> aggregate_objects;
	//! The minimum value of each of the groups
	vector<Value> group_minima;
	//! The number of bits we need to completely cover each of the groups
	vector<idx_t> required_bits;

	unordered_map<Expression *, size_t> filter_indexes;
};

} // namespace goose
