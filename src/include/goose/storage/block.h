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

#include <goose/common/common.h>
#include <goose/common/file_buffer.h>
#include <goose/storage/storage_info.h>

namespace goose {

class BlockAllocator;
class Serializer;
class Deserializer;

class Block : public FileBuffer {
public:
	Block(BlockAllocator &allocator, const block_id_t id, const idx_t block_size, const idx_t block_header_size);
	Block(BlockAllocator &allocator, block_id_t id, uint32_t internal_size, idx_t block_header_size);
	Block(BlockAllocator &allocator, const block_id_t id, BlockManager &block_manager);
	Block(FileBuffer &source, block_id_t id, idx_t block_header_size);

	block_id_t id;
};

struct BlockPointer {
	BlockPointer(block_id_t block_id_p, uint32_t offset_p) : block_id(block_id_p), offset(offset_p) {
	}
	BlockPointer() : block_id(INVALID_BLOCK), offset(0) {
	}

	block_id_t block_id;
	uint32_t offset;
	uint32_t unused_padding {0};

	bool IsValid() const {
		return block_id != INVALID_BLOCK;
	}

	void Serialize(Serializer &serializer) const;
	static BlockPointer Deserialize(Deserializer &source);
};

struct MetaBlockPointer {
	MetaBlockPointer(idx_t block_pointer, uint32_t offset_p) : block_pointer(block_pointer), offset(offset_p) {
	}
	MetaBlockPointer() : block_pointer(DConstants::INVALID_INDEX), offset(0) {
	}

	idx_t block_pointer;
	uint32_t offset;
	uint32_t unused_padding {0};

	bool IsValid() const {
		return block_pointer != DConstants::INVALID_INDEX;
	}
	block_id_t GetBlockId() const;
	uint32_t GetBlockIndex() const;

	bool operator==(const MetaBlockPointer &rhs) const {
		return block_pointer == rhs.block_pointer && offset == rhs.offset;
	}

	friend std::ostream &operator<<(std::ostream &os, const MetaBlockPointer &obj) {
		return os << "{block_id: " << obj.GetBlockId() << " index: " << obj.GetBlockIndex() << " offset: " << obj.offset
		          << "}";
	}

	void Serialize(Serializer &serializer) const;
	static MetaBlockPointer Deserialize(Deserializer &source);
};

} // namespace goose
