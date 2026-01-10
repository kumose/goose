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

#include <goose/execution/expression_executor.h>
#include <goose/function/aggregate_function.h>
#include <goose/planner/expression/bound_aggregate_expression.h>
#include <goose/execution/operator/aggregate/aggregate_object.h>

namespace goose {
struct DistinctAggregateData;
struct LocalUngroupedAggregateState;

struct UngroupedAggregateState {
	explicit UngroupedAggregateState(const vector<unique_ptr<Expression>> &aggregate_expressions);
	~UngroupedAggregateState();

	void Move(UngroupedAggregateState &other);

public:
	//! Aggregates
	const vector<unique_ptr<Expression>> &aggregate_expressions;
	//! The aggregate values
	vector<unsafe_unique_array<data_t>> aggregate_data;
	//! The bind data
	vector<optional_ptr<FunctionData>> bind_data;
	//! The destructors
	vector<aggregate_destructor_t> destructors;
	//! Counts (used for verification)
	unique_array<atomic<idx_t>> counts;
};

struct GlobalUngroupedAggregateState {
public:
	GlobalUngroupedAggregateState(Allocator &client_allocator, const vector<unique_ptr<Expression>> &aggregates)
	    : client_allocator(client_allocator), allocator(client_allocator), state(aggregates) {
	}

	mutable mutex lock;
	//! Client allocator
	Allocator &client_allocator;
	//! Global arena allocator
	ArenaAllocator allocator;
	//! Allocator pool
	mutable vector<unique_ptr<ArenaAllocator>> stored_allocators;
	//! The global aggregate state
	UngroupedAggregateState state;

public:
	//! Create an ArenaAllocator with cross-thread lifetime
	ArenaAllocator &CreateAllocator() const;
	void Combine(LocalUngroupedAggregateState &other);
	void CombineDistinct(LocalUngroupedAggregateState &other, DistinctAggregateData &distinct_data);
	void Finalize(DataChunk &result, idx_t column_offset = 0);
};

struct LocalUngroupedAggregateState {
public:
	explicit LocalUngroupedAggregateState(GlobalUngroupedAggregateState &gstate);

	//! Local arena allocator
	ArenaAllocator &allocator;
	//! The local aggregate state
	UngroupedAggregateState state;

public:
	void Sink(DataChunk &payload_chunk, idx_t payload_idx, idx_t aggr_idx);
};

struct UngroupedAggregateExecuteState {
public:
	UngroupedAggregateExecuteState(ClientContext &context, const vector<unique_ptr<Expression>> &aggregates,
	                               const vector<LogicalType> &child_types);

	//! The set of aggregates
	const vector<unique_ptr<Expression>> &aggregates;
	//! The executor
	ExpressionExecutor child_executor;
	//! The payload chunk, containing all the Vectors for the aggregates
	DataChunk aggregate_input_chunk;
	//! Aggregate filter data set
	AggregateFilterDataSet filter_set;

public:
	void Sink(LocalUngroupedAggregateState &state, DataChunk &input);
	void Reset();
};

} // namespace goose
