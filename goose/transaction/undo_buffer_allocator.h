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

#include <goose/common/common.h>
#include <goose/storage/buffer/buffer_handle.h>
#include <goose/common/types-import.h>

namespace goose {
class BufferManager;
class BlockHandle;
struct UndoBufferEntry;
struct UndoBufferPointer;

struct UndoBufferEntry {
	explicit UndoBufferEntry(BufferManager &buffer_manager) : buffer_manager(buffer_manager) {
	}
	~UndoBufferEntry();

	BufferManager &buffer_manager;
	shared_ptr<BlockHandle> block;
	idx_t position = 0;
	idx_t capacity = 0;
	unique_ptr<UndoBufferEntry> next;
	optional_ptr<UndoBufferEntry> prev;
};

struct UndoBufferReference {
	UndoBufferReference() : entry(nullptr), position(0) {
	}
	UndoBufferReference(UndoBufferEntry &entry_p, BufferHandle handle_p, idx_t position)
	    : entry(&entry_p), handle(std::move(handle_p)), position(position) {
	}

	optional_ptr<UndoBufferEntry> entry;
	BufferHandle handle;
	idx_t position;

	data_ptr_t Ptr() {
		return handle.Ptr() + position;
	}
	bool IsSet() const {
		return entry;
	}

	UndoBufferPointer GetBufferPointer();
};

struct UndoBufferPointer {
	UndoBufferPointer() : entry(nullptr), position(0) {
	}
	UndoBufferPointer(UndoBufferEntry &entry_p, idx_t position) : entry(&entry_p), position(position) {
	}

	UndoBufferEntry *entry;
	idx_t position;

	UndoBufferReference Pin() const;
	bool IsSet() const {
		return entry;
	}
};

struct UndoBufferAllocator {
	explicit UndoBufferAllocator(BufferManager &buffer_manager);

	UndoBufferReference Allocate(idx_t alloc_len);

	mutex lock;
	BufferManager &buffer_manager;
	unique_ptr<UndoBufferEntry> head;
	optional_ptr<UndoBufferEntry> tail;
};

} // namespace goose
