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

#include <goose/execution/operator/aggregate/grouped_aggregate_data.h>
#include <goose/execution/radix_partitioned_hashtable.h>

namespace goose {

class GroupedAggregateData;

struct DistinctAggregateCollectionInfo {
public:
	DistinctAggregateCollectionInfo(const vector<unique_ptr<Expression>> &aggregates, vector<idx_t> indices);

public:
	// The indices of the aggregates that are distinct
	unsafe_vector<idx_t> indices;
	// The amount of radix_tables that are occupied
	idx_t table_count;
	//! Occupied tables, not equal to indices if aggregates share input data
	vector<idx_t> table_indices;
	//! This indirection is used to allow two aggregates to share the same input data
	unordered_map<idx_t, idx_t> table_map;
	const vector<unique_ptr<Expression>> &aggregates;
	// Total amount of children of the distinct aggregates
	idx_t total_child_count;

public:
	static unique_ptr<DistinctAggregateCollectionInfo> Create(vector<unique_ptr<Expression>> &aggregates);
	const unsafe_vector<idx_t> &Indices() const;
	bool AnyDistinct() const;

private:
	//! Returns the amount of tables that are occupied
	idx_t CreateTableIndexMap();
};

struct DistinctAggregateData {
public:
	DistinctAggregateData(const DistinctAggregateCollectionInfo &info, TupleDataValidityType distinct_validity);
	DistinctAggregateData(const DistinctAggregateCollectionInfo &info, const GroupingSet &groups,
	                      const vector<unique_ptr<Expression>> *group_expressions,
	                      TupleDataValidityType distinct_validity);
	//! The data used by the hashtables
	vector<unique_ptr<GroupedAggregateData>> grouped_aggregate_data;
	//! The hashtables
	vector<unique_ptr<RadixPartitionedHashTable>> radix_tables;
	//! The groups (arguments)
	vector<GroupingSet> grouping_sets;
	const DistinctAggregateCollectionInfo &info;

public:
	bool IsDistinct(idx_t index) const;
};

struct DistinctAggregateState {
public:
	DistinctAggregateState(const DistinctAggregateData &data, ClientContext &client);

	//! The executor
	ExpressionExecutor child_executor;
	//! The global sink states of the hash tables
	vector<unique_ptr<GlobalSinkState>> radix_states;
	//! Output chunks to receive distinct data from hashtables
	vector<unique_ptr<DataChunk>> distinct_output_chunks;
};

} // namespace goose
