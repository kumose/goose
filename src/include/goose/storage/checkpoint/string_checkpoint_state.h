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
#include <goose/storage/storage_info.h>
#include <goose/storage/buffer/block_handle.h>
#include <goose/function/compression_function.h>

namespace goose {
struct UncompressedStringSegmentState;

class OverflowStringWriter {
public:
	virtual ~OverflowStringWriter() {
	}

	virtual void WriteString(UncompressedStringSegmentState &state, string_t string, block_id_t &result_block,
	                         int32_t &result_offset) = 0;
	virtual void Flush() = 0;
};

struct StringBlock {
	shared_ptr<BlockHandle> block;
	idx_t offset;
	idx_t size;
	unique_ptr<StringBlock> next;
};

struct UncompressedStringSegmentState : public CompressedSegmentState {
	~UncompressedStringSegmentState() override;

	//! The string block holding strings that do not fit in the main block
	//! FIXME: this should be replaced by a heap that also allows freeing of unused strings
	unique_ptr<StringBlock> head;
	//! Map of block id to string block
	unordered_map<block_id_t, reference<StringBlock>> overflow_blocks;
	//! Overflow string writer (if any), if not set overflow strings will be written to memory blocks
	unique_ptr<OverflowStringWriter> overflow_writer;
	//! The block manager with which to write
	optional_ptr<BlockManager> block_manager;
	//! The set of overflow blocks written to disk (if any)
	vector<block_id_t> on_disk_blocks;

public:
	shared_ptr<BlockHandle> GetHandle(BlockManager &manager, block_id_t block_id);

	void RegisterBlock(BlockManager &manager, block_id_t block_id);

	string GetSegmentInfo() const override;

private:
	mutex block_lock;
	unordered_map<block_id_t, shared_ptr<BlockHandle>> handles;
};

} // namespace goose
