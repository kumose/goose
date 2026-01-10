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

#include <goose/common/types-import.h>
#include <goose/common/types/column/column_data_collection.h>
#include <goose/main/query_parameters.h>

namespace goose {

class BufferManager;
class ClientContext;

using batch_map_t = map<idx_t, unique_ptr<ColumnDataCollection>>;
using batch_iterator_t = typename batch_map_t::iterator;

struct BatchedChunkIteratorRange {
	batch_iterator_t begin;
	batch_iterator_t end;
};

struct BatchedChunkScanState {
	BatchedChunkIteratorRange range;
	ColumnDataScanState scan_state;
};

//!  A BatchedDataCollection holds a number of data entries that are partitioned by batch index
//! Scans over a BatchedDataCollection are ordered by batch index
class BatchedDataCollection {
public:
	GOOSE_API
	BatchedDataCollection(ClientContext &context, vector<LogicalType> types,
	                      ColumnDataAllocatorType allocator_type = ColumnDataAllocatorType::IN_MEMORY_ALLOCATOR,
	                      ColumnDataCollectionLifetime lifetime = ColumnDataCollectionLifetime::REGULAR);
	GOOSE_API
	BatchedDataCollection(ClientContext &context, vector<LogicalType> types, QueryResultMemoryType memory_type);
	GOOSE_API
	BatchedDataCollection(ClientContext &context, vector<LogicalType> types, batch_map_t batches,
	                      ColumnDataAllocatorType allocator_type = ColumnDataAllocatorType::IN_MEMORY_ALLOCATOR,
	                      ColumnDataCollectionLifetime lifetime = ColumnDataCollectionLifetime::REGULAR);

	//! Appends a datachunk with the given batch index to the batched collection
	GOOSE_API void Append(DataChunk &input, idx_t batch_index);

	//! Merge the other batched chunk collection into this batched collection
	GOOSE_API void Merge(BatchedDataCollection &other);

	// Initialize a scan over a range of batches of the batched chunk collection
	void InitializeScan(BatchedChunkScanState &state, const BatchedChunkIteratorRange &range);

	//! Initialize a scan over the batched chunk collection
	GOOSE_API void InitializeScan(BatchedChunkScanState &state);

	//! Scan a chunk from the batched chunk collection, in-order of batch index
	GOOSE_API void Scan(BatchedChunkScanState &state, DataChunk &output);

	//! Fetch a column data collection from the batched data collection - this consumes all of the data stored within
	GOOSE_API unique_ptr<ColumnDataCollection> FetchCollection();

	//! Inspect how many tuples this batched data collection contains
	GOOSE_API idx_t Count() const;

	//! Inspect the types of the collection
	GOOSE_API const vector<LogicalType> &Types() const;

	//! Inspect how many batches this collection contains
	GOOSE_API idx_t BatchCount() const;

	//! Retrieve the batch index of the nth batch in the collection
	GOOSE_API idx_t IndexToBatchIndex(idx_t index) const;

	//! Inspect how big a given batch is
	GOOSE_API idx_t BatchSize(idx_t batch_index) const;

	//! Inspect a given batch through a const reference
	const ColumnDataCollection &Batch(idx_t batch_index) const;

	//! Create an iterator range from the provided indices
	BatchedChunkIteratorRange BatchRange(idx_t begin = 0, idx_t end = DConstants::INVALID_INDEX);

	GOOSE_API string ToString() const;
	GOOSE_API void Print() const;

private:
	unique_ptr<ColumnDataCollection> CreateCollection() const;

	struct CachedCollection {
		idx_t batch_index = DConstants::INVALID_INDEX;
		ColumnDataCollection *collection = nullptr;
		ColumnDataAppendState append_state;
	};

	ClientContext &context;
	vector<LogicalType> types;
	ColumnDataAllocatorType allocator_type;
	ColumnDataCollectionLifetime lifetime;
	//! The data of the batched chunk collection - a set of batch_index -> ColumnDataCollection pointers
	map<idx_t, unique_ptr<ColumnDataCollection>> data;
	//! The last batch collection that was inserted into
	CachedCollection last_collection;
};

} // namespace goose
