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

#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>

namespace goose {

enum class ColumnDataAllocatorType : uint8_t {
	//! Use a buffer manager to allocate large chunks of memory that vectors then use
	BUFFER_MANAGER_ALLOCATOR,
	//! Use an in-memory allocator, allocating data for every chunk
	//! This causes the column data collection to allocate blocks that are not tied to a buffer manager
	IN_MEMORY_ALLOCATOR,
	//! Use a buffer manager to allocate vectors, but use a StringHeap for strings
	HYBRID
};

enum class ColumnDataScanProperties : uint8_t {
	INVALID,
	//! Allow zero copy scans - this introduces a dependency on the resulting vector on the scan state of the column
	//! data collection, which means vectors might not be valid anymore after the next chunk is scanned.
	ALLOW_ZERO_COPY,
	//! Disallow zero-copy scans, always copying data into the target vector
	//! As a result, data scanned will be valid even after the column data collection is destroyed
	DISALLOW_ZERO_COPY
};

enum class ColumnDataCollectionLifetime {
	//! Regular lifetime management
	REGULAR,
	//! Accessing will throw an error after the DB closes
	//! Optional for ColumnDataAllocatorType::BUFFER_MANAGER_ALLOCATOR only
	THROW_ERROR_AFTER_DATABASE_CLOSES,
};

struct ChunkManagementState {
	unordered_map<idx_t, BufferHandle> handles;
	ColumnDataScanProperties properties = ColumnDataScanProperties::INVALID;
};

struct ColumnDataAppendState {
	ChunkManagementState current_chunk_state;
	vector<UnifiedVectorFormat> vector_data;
};

struct ColumnDataScanState {
	//! Database instance if scanning ColumnDataCollectionLifetime::DATABASE_INSTANCE
	shared_ptr<DatabaseInstance> db;

	ChunkManagementState current_chunk_state;
	idx_t segment_index;
	idx_t chunk_index;
	idx_t current_row_index;
	idx_t next_row_index;
	ColumnDataScanProperties properties;
	vector<column_t> column_ids;
};

struct ColumnDataParallelScanState {
	ColumnDataScanState scan_state;
	mutex lock;
};

struct ColumnDataLocalScanState {
	ChunkManagementState current_chunk_state;
	idx_t current_segment_index = DConstants::INVALID_INDEX;
	idx_t current_row_index;
};

class ColumnDataRow {
public:
	ColumnDataRow(DataChunk &chunk, idx_t row_index, idx_t base_index);

	DataChunk &chunk;
	idx_t row_index;
	idx_t base_index;

public:
	Value GetValue(idx_t column_index) const;
	idx_t RowIndex() const;
};

} // namespace goose
