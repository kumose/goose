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
#include <goose/storage/block.h>
#include <goose/storage/block_manager.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/storage/buffer/buffer_handle.h>

namespace goose {
class DatabaseInstance;
struct MetadataBlockInfo;

struct MetadataBlock {
	MetadataBlock();
	// disable copy constructors
	MetadataBlock(const MetadataBlock &other) = delete;
	MetadataBlock &operator=(const MetadataBlock &) = delete;
	//! enable move constructors
	GOOSE_API MetadataBlock(MetadataBlock &&other) noexcept;
	GOOSE_API MetadataBlock &operator=(MetadataBlock &&) noexcept;

	shared_ptr<BlockHandle> block;
	block_id_t block_id;
	vector<uint8_t> free_blocks;
	atomic<bool> dirty;

	void Write(WriteStream &sink);
	static MetadataBlock Read(ReadStream &source);

	idx_t FreeBlocksToInteger();
	void FreeBlocksFromInteger(idx_t blocks);
	static vector<uint8_t> BlocksFromInteger(idx_t free_list);

	string ToString() const;
};

struct MetadataPointer {
	idx_t block_index : 56;
	uint8_t index : 8;
};

struct MetadataHandle {
	MetadataPointer pointer;
	BufferHandle handle;
};

class MetadataManager {
public:
	//! The amount of metadata blocks per storage block
	static constexpr const idx_t METADATA_BLOCK_COUNT = 64;

public:
	MetadataManager(BlockManager &block_manager, BufferManager &buffer_manager);
	~MetadataManager();

	BufferManager &GetBufferManager() const {
		return buffer_manager;
	}

	MetadataHandle AllocateHandle();
	MetadataHandle Pin(const MetadataPointer &pointer);

	MetadataHandle Pin(const QueryContext &context, const MetadataPointer &pointer);

	MetaBlockPointer GetDiskPointer(const MetadataPointer &pointer, uint32_t offset = 0);
	MetadataPointer FromDiskPointer(MetaBlockPointer pointer);
	MetadataPointer RegisterDiskPointer(MetaBlockPointer pointer);

	static BlockPointer ToBlockPointer(MetaBlockPointer meta_pointer, const idx_t metadata_block_size);
	static MetaBlockPointer FromBlockPointer(BlockPointer block_pointer, const idx_t metadata_block_size);

	//! Flush all blocks to disk
	void Flush();

	bool BlockHasBeenCleared(const MetaBlockPointer &ptr);

	void MarkBlocksAsModified();
	void ClearModifiedBlocks(const vector<MetaBlockPointer> &pointers);

	vector<MetadataBlockInfo> GetMetadataInfo() const;
	vector<shared_ptr<BlockHandle>> GetBlocks() const;
	idx_t BlockCount();

	void Write(WriteStream &sink);
	void Read(ReadStream &source);

	idx_t GetMetadataBlockSize() const;

protected:
	BlockManager &block_manager;
	BufferManager &buffer_manager;
	mutable mutex block_lock;
	unordered_map<block_id_t, MetadataBlock> blocks;
	unordered_map<block_id_t, idx_t> modified_blocks;

protected:
	block_id_t AllocateNewBlock(unique_lock<mutex> &block_lock);
	block_id_t PeekNextBlockId() const;
	block_id_t GetNextBlockId() const;

	void AddBlock(unique_lock<mutex> &block_lock, MetadataBlock new_block, bool if_exists = false);
	void AddAndRegisterBlock(unique_lock<mutex> &block_lock, MetadataBlock block);
	void ConvertToTransient(unique_lock<mutex> &block_lock, MetadataBlock &block);
	MetadataPointer FromDiskPointerInternal(unique_lock<mutex> &block_lock, MetaBlockPointer pointer);
};

} // namespace goose
