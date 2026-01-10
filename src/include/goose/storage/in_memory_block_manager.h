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
#include <goose/common/exception.h>
#include <goose/main/client_context.h>
#include <goose/storage/block_manager.h>

namespace goose {

//! InMemoryBlockManager is an implementation for a BlockManager
class InMemoryBlockManager : public BlockManager {
public:
	using BlockManager::BlockManager;

	// LCOV_EXCL_START
	unique_ptr<Block> ConvertBlock(block_id_t block_id, FileBuffer &source_buffer) override {
		throw InternalException("Cannot perform IO in in-memory database - ConvertBlock!");
	}
	unique_ptr<Block> CreateBlock(block_id_t block_id, FileBuffer *source_buffer) override {
		throw InternalException("Cannot perform IO in in-memory database - CreateBlock!");
	}
	block_id_t GetFreeBlockId() override {
		throw InternalException("Cannot perform IO in in-memory database - GetFreeBlockId!");
	}
	block_id_t GetFreeBlockIdForCheckpoint() override {
		throw InternalException("Cannot perform IO in in-memory database - GetFreeBlockIdForCheckpoint!");
	}
	block_id_t PeekFreeBlockId() override {
		throw InternalException("Cannot perform IO in in-memory database - PeekFreeBlockId!");
	}
	bool IsRootBlock(MetaBlockPointer root) override {
		throw InternalException("Cannot perform IO in in-memory database - IsRootBlock!");
	}
	void MarkBlockACheckpointed(block_id_t block_id) override {
		throw InternalException("Cannot perform IO in in-memory database - MarkBlockACheckpointed!");
	}
	void MarkBlockAsUsed(block_id_t block_id) override {
		throw InternalException("Cannot perform IO in in-memory database - MarkBlockAsUsed!");
	}
	void MarkBlockAsModified(block_id_t block_id) override {
		throw InternalException("Cannot perform IO in in-memory database - MarkBlockAsModified!");
	}
	void IncreaseBlockReferenceCount(block_id_t block_id) override {
		throw InternalException("Cannot perform IO in in-memory database - IncreaseBlockReferenceCount!");
	}
	idx_t GetMetaBlock() override {
		throw InternalException("Cannot perform IO in in-memory database - GetMetaBlock!");
	}

	void Read(QueryContext context, Block &block) override {
		throw InternalException("Cannot perform IO in in-memory database - Read!");
	}
	void ReadBlocks(FileBuffer &buffer, block_id_t start_block, idx_t block_count) override {
		throw InternalException("Cannot perform IO in in-memory database - ReadBlocks!");
	}
	void Write(FileBuffer &block, block_id_t block_id) override {
		throw InternalException("Cannot perform IO in in-memory database - Write!");
	}
	void Write(QueryContext context, FileBuffer &block, block_id_t block_id) override {
		throw InternalException("Cannot perform IO in in-memory database - Write with client context!");
	}
	void WriteHeader(QueryContext context, DatabaseHeader header) override {
		throw InternalException("Cannot perform IO in in-memory database - WriteHeader!");
	}
	bool InMemory() override {
		return true;
	}
	void FileSync() override {
		throw InternalException("Cannot perform IO in in-memory database - FileSync!");
	}
	idx_t TotalBlocks() override {
		throw InternalException("Cannot perform IO in in-memory database - TotalBlocks!");
	}
	idx_t FreeBlocks() override {
		throw InternalException("Cannot perform IO in in-memory database - FreeBlocks!");
	}
	// LCOV_EXCL_STOP
};
} // namespace goose
