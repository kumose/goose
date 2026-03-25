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
#include <goose/common/types/column/column_data_collection_segment.h>
#include <goose/common/types/column/column_data_scan_states.h>

namespace goose {

struct ColumnDataConsumerScanState {
	ColumnDataAllocator *allocator = nullptr;
	ChunkManagementState current_chunk_state;
	idx_t chunk_index;
};

//! ColumnDataConsumer can scan a ColumnDataCollection, and consume it in the process, i.e., read blocks are deleted
class ColumnDataConsumer {
public:
	struct ChunkReference {
	public:
		ChunkReference(ColumnDataCollectionSegment *segment_p, uint32_t chunk_index_p);
		uint32_t GetMinimumBlockID() const;
		friend bool operator<(const ChunkReference &lhs, const ChunkReference &rhs) {
			// Sort by allocator first
			if (lhs.segment->allocator.get() != rhs.segment->allocator.get()) {
				return lhs.segment->allocator.get() < rhs.segment->allocator.get();
			}
			// Then by minimum block id
			return lhs.GetMinimumBlockID() < rhs.GetMinimumBlockID();
		}

	public:
		ColumnDataCollectionSegment *segment;
		uint32_t chunk_index_in_segment;
	};

public:
	ColumnDataConsumer(ColumnDataCollection &collection, vector<column_t> column_ids);

	idx_t Count() const {
		return collection.Count();
	}

	idx_t ChunkCount() const {
		return chunk_count;
	}

public:
	//! Initialize the scan of the ColumnDataCollection
	void InitializeScan();
	//! Assign a chunk to the scan state
	bool AssignChunk(ColumnDataConsumerScanState &state);
	//! Scan the assigned chunk
	void ScanChunk(ColumnDataConsumerScanState &state, DataChunk &chunk) const;
	//! Indicate that scanning the chunk is done
	void FinishChunk(ColumnDataConsumerScanState &state);

private:
	void ConsumeChunks(idx_t delete_index_start, idx_t delete_index_end);

private:
	mutex lock;
	//! The collection being scanned
	ColumnDataCollection &collection;
	//! The column ids to scan
	vector<column_t> column_ids;
	//! The number of chunk references
	idx_t chunk_count;
	//! The chunks (in order) to be scanned
	vector<ChunkReference> chunk_references;
	//! Current index into "chunks"
	idx_t current_chunk_index;
	//! Chunks currently in progress
	unordered_set<idx_t> chunks_in_progress;
	//! The data has been consumed up to this chunk index
	idx_t chunk_delete_index;
};

} // namespace goose
