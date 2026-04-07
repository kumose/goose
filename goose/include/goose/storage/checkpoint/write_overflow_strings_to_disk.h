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
