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
