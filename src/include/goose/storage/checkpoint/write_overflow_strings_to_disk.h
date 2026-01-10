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

#include <goose/storage/checkpoint/string_checkpoint_state.h>

namespace goose {

class WriteOverflowStringsToDisk : public OverflowStringWriter {
public:
	explicit WriteOverflowStringsToDisk(PartialBlockManager &partial_block_manager);
	~WriteOverflowStringsToDisk() override;

	//! The block manager
	PartialBlockManager &partial_block_manager;

	//! Temporary buffer
	BufferHandle handle;
	//! The block on-disk to which we are writing
	block_id_t block_id;
	//! The offset within the current block
	idx_t offset;

public:
	void WriteString(UncompressedStringSegmentState &state, string_t string, block_id_t &result_block,
	                 int32_t &result_offset) override;
	void Flush() override;

private:
	void AllocateNewBlock(UncompressedStringSegmentState &state, block_id_t new_block_id);
	idx_t GetStringSpace() const;
};

} // namespace goose
