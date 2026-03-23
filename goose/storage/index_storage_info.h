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

#include <goose/common/case_insensitive_map.h>
#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/storage/block.h>
#include <goose/storage/storage_info.h>

namespace goose {

//! Information to serialize a FixedSizeAllocator, which holds the index data
struct FixedSizeAllocatorInfo {
	idx_t segment_size;
	vector<idx_t> buffer_ids;
	vector<BlockPointer> block_pointers;
	vector<idx_t> segment_counts;
	vector<idx_t> allocation_sizes;
	vector<idx_t> buffers_with_free_space;

	void Serialize(Serializer &serializer) const;
	static FixedSizeAllocatorInfo Deserialize(Deserializer &deserializer);
};

//! Information to serialize an index buffer to the WAL
struct IndexBufferInfo {
	IndexBufferInfo(data_ptr_t buffer_ptr, const idx_t allocation_size)
	    : buffer_ptr(buffer_ptr), allocation_size(allocation_size) {
	}

	data_ptr_t buffer_ptr;
	idx_t allocation_size;
};

//! Index (de)serialization information.
struct IndexStorageInfo {
	IndexStorageInfo() {};
	explicit IndexStorageInfo(const string &name) : name(name) {};

	//! The name.
	string name;
	//! The storage root.
	idx_t root;
	//! Any index specialization can provide additional key-Value settings via this map.
	case_insensitive_map_t<Value> options;
	//! Serialization information for fixed-size allocator memory.
	vector<FixedSizeAllocatorInfo> allocator_infos;

	//! Contains all buffer pointers and their allocation size for serializing to the WAL.
	//! First dimension: All fixed-size allocators.
	//! Second dimension: The buffers of each fixed-size allocator.
	vector<vector<IndexBufferInfo>> buffers;

	//! The root block pointer of the index. Necessary to support older storage files.
	BlockPointer root_block_ptr;

	//! Returns true, if IndexStorageInfo holds information to deserialize an index.
	//! Note that the name can be misleading - any index that is empty (no nodes, etc.) might
	//! also have neither a root_block_ptr nor allocator_infos.
	//! Ensure that your index constructor initializes an empty index correctly without the
	//! need for these fields.
	bool IsValid() const {
		return root_block_ptr.IsValid() || !allocator_infos.empty();
	}

	void Serialize(Serializer &serializer) const;
	static IndexStorageInfo Deserialize(Deserializer &deserializer);
};

//! Additional index information for tables
struct IndexInfo {
	bool is_unique;
	bool is_primary;
	bool is_foreign;
	unordered_set<column_t> column_set;
};

} // namespace goose
