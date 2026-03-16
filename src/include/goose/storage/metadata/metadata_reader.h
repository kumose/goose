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

#include <goose/storage/metadata/metadata_manager.h>
#include <goose/common/serializer/read_stream.h>

namespace goose {

enum class BlockReaderType { EXISTING_BLOCKS, REGISTER_BLOCKS };

class MetadataReader : public ReadStream {
public:
	MetadataReader(MetadataManager &manager, MetaBlockPointer pointer,
	               optional_ptr<vector<MetaBlockPointer>> read_pointers = nullptr,
	               BlockReaderType type = BlockReaderType::EXISTING_BLOCKS);
	MetadataReader(MetadataManager &manager, BlockPointer pointer);
	~MetadataReader() override;

public:
	//! Read content of size read_size into the buffer
	void ReadData(data_ptr_t buffer, idx_t read_size) override;

	void ReadData(QueryContext context, data_ptr_t buffer, idx_t read_size) override;

	MetaBlockPointer GetMetaBlockPointer();

	MetadataManager &GetMetadataManager() {
		return manager;
	}
	//! Gets a list of all remaining blocks to be read by this metadata reader - consumes all blocks
	//! If "last_block" is specified, we stop when reaching that block
	vector<MetaBlockPointer> GetRemainingBlocks(MetaBlockPointer last_block = MetaBlockPointer());

private:
	data_ptr_t BasePtr();
	data_ptr_t Ptr();

	void ReadNextBlock();

	void ReadNextBlock(QueryContext context);

	MetadataPointer FromDiskPointer(MetaBlockPointer pointer);

private:
	MetadataManager &manager;
	BlockReaderType type;
	MetadataHandle block;
	MetaBlockPointer next_pointer;
	bool has_next_block;
	optional_ptr<vector<MetaBlockPointer>> read_pointers;
	idx_t index;
	idx_t offset;
	idx_t next_offset;
	idx_t capacity;
};

} // namespace goose
