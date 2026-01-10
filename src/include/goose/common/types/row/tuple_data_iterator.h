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

#include <goose/common/types/row/tuple_data_collection.h>

namespace goose {

class TupleDataChunkIterator {
public:
	//! Creates a TupleDataChunkIterator that iterates over all DataChunks in the TupleDataCollection
	TupleDataChunkIterator(TupleDataCollection &collection, TupleDataPinProperties properties, bool init_heap);
	//! Creates a TupleDataChunkIterator that iterates over the specified DataChunk range in the TupleDataCollection
	TupleDataChunkIterator(TupleDataCollection &collection, TupleDataPinProperties properties, idx_t chunk_idx_from,
	                       idx_t chunk_idx_to, bool init_heap);

public:
	//! Whether the iterator is done
	bool Done() const;
	//! Fetches the next STANDARD_VECTOR_SIZE row locations (and heap locations/sizes if init_heap is true)
	bool Next();
	//! Resets the scan indices to the start
	void Reset();
	//! Get the count of the current "DataChunk"
	idx_t GetCurrentChunkCount() const;
	//! Get the Chunk state of the scan state of this iterator
	TupleDataChunkState &GetChunkState();
	//! Get the array holding the row locations
	data_ptr_t *GetRowLocations();
	//! Get the array holding the heap locations
	data_ptr_t *GetHeapLocations();
	//! Get the array holding the heap sizes
	idx_t *GetHeapSizes();

private:
	//! Initializes the row locations (and heap locations/sizes if init_heap is true) at the current scan indices
	void InitializeCurrentChunk();

private:
	//! The collection being iterated over
	TupleDataCollection &collection;
	//! Whether or not to fetch the heap locations/sizes while iterating
	bool init_heap;

	//! Start indices
	idx_t start_segment_idx;
	idx_t start_chunk_idx;
	//! End indices
	idx_t end_segment_idx;
	idx_t end_chunk_idx;

	//! Current scan state and scan indices
	TupleDataScanState state;
	idx_t current_segment_idx;
	idx_t current_chunk_idx;
};

} // namespace goose
