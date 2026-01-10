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

#include <goose/execution/base_aggregate_hashtable.h>
#include <goose/storage/arena_allocator.h>

namespace goose {

class PerfectAggregateHashTable : public BaseAggregateHashTable {
public:
	PerfectAggregateHashTable(ClientContext &context, Allocator &allocator, const vector<LogicalType> &group_types,
	                          vector<LogicalType> payload_types_p, vector<AggregateObject> aggregate_objects,
	                          vector<Value> group_minima, vector<idx_t> required_bits);
	~PerfectAggregateHashTable() override;

public:
	//! Add the given data to the HT
	void AddChunk(DataChunk &groups, DataChunk &payload);

	//! Combines the target perfect aggregate HT into this one
	void Combine(PerfectAggregateHashTable &other);

	//! Scan the HT starting from the scan_position
	void Scan(idx_t &scan_position, DataChunk &result);

protected:
	Vector addresses;
	//! The required bits per group
	vector<idx_t> required_bits;
	//! The total required bits for the HT (this determines the max capacity)
	idx_t total_required_bits;
	//! The total amount of groups
	idx_t total_groups;
	//! The tuple size
	idx_t tuple_size;
	//! The number of grouping columns
	idx_t grouping_columns;

	// The actual pointer to the data
	data_ptr_t data;
	//! The owned data of the HT
	unsafe_unique_array<data_t> owned_data;
	//! Information on whether or not a specific group has any entries
	unsafe_unique_array<bool> group_is_set;

	//! The minimum values for each of the group columns
	vector<Value> group_minima;

	//! Reused selection vector
	SelectionVector sel;

	//! The active arena allocator used by the aggregates for their internal state
	unique_ptr<ArenaAllocator> aggregate_allocator;
	//! Owning arena allocators that this HT has data from
	vector<unique_ptr<ArenaAllocator>> stored_allocators;

private:
	//! Destroy the perfect aggregate HT (called automatically by the destructor)
	void Destroy();
};

} // namespace goose
