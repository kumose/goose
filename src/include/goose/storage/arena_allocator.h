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

#include <goose/common/allocator.h>
#include <goose/common/common.h>
#include <goose/common/types/string.h>
#include <goose/common/arena_containers/arena_ptr.h>

namespace goose {

struct ArenaChunk {
	ArenaChunk(Allocator &allocator, idx_t size);
	~ArenaChunk();

	AllocatedData data;
	idx_t current_position;
	idx_t maximum_size;
	unsafe_unique_ptr<ArenaChunk> next;
	ArenaChunk *prev;
};

class ArenaAllocator {
public:
	static constexpr const idx_t ARENA_ALLOCATOR_INITIAL_CAPACITY = 2048;
	static constexpr const idx_t ARENA_ALLOCATOR_MAX_CAPACITY = 1ULL << 24ULL; // 16MB

public:
	GOOSE_API explicit ArenaAllocator(Allocator &allocator, idx_t initial_capacity = ARENA_ALLOCATOR_INITIAL_CAPACITY);
	GOOSE_API ~ArenaAllocator();

	data_ptr_t Allocate(idx_t len) {
		D_ASSERT(!head || head->current_position <= head->maximum_size);
		if (!head || head->current_position + len > head->maximum_size) {
			AllocateNewBlock(len);
		}
		D_ASSERT(head->current_position + len <= head->maximum_size);
		auto result = head->data.get() + head->current_position;
		head->current_position += len;
		return result;
	}
	GOOSE_API data_ptr_t Reallocate(data_ptr_t pointer, idx_t old_size, idx_t size);

	GOOSE_API data_ptr_t AllocateAligned(idx_t size);
	GOOSE_API data_ptr_t ReallocateAligned(data_ptr_t pointer, idx_t old_size, idx_t size);

	//! Increment the internal cursor (if required) so the next allocation is guaranteed to be aligned to 8 bytes
	GOOSE_API void AlignNext();

	//! This shrinks the LAST allocation that was made using the allocator
	//! Note that we can ONLY safely call this method if Allocate has been called previously with a size >= shrink_size
	GOOSE_API void ShrinkHead(idx_t shrink_size) const {
		D_ASSERT(head && head->current_position >= shrink_size);
		head->current_position -= shrink_size;
	}

	//! Resets the current head and destroys all previous arena chunks
	GOOSE_API void Reset();
	GOOSE_API void Destroy();
	GOOSE_API void Move(ArenaAllocator &allocator);

	GOOSE_API ArenaChunk *GetHead();
	GOOSE_API ArenaChunk *GetTail();

	GOOSE_API bool IsEmpty() const;
	//! Get the total *used* size (not cached)
	GOOSE_API idx_t SizeInBytes() const;
	//! Get the currently allocated size in bytes (cached, read from "allocated_size")
	GOOSE_API idx_t AllocationSize() const;

	//! Returns an "Allocator" wrapper for this arena allocator
	Allocator &GetAllocator() {
		return arena_allocator;
	}

	template <class T, class... ARGS>
	T *Make(ARGS &&... args) {
		auto mem = AllocateAligned(sizeof(T));
		return new (mem) T(std::forward<ARGS>(args)...);
	}

	template <class T, class... ARGS>
	arena_ptr<T> MakePtr(ARGS &&... args) {
		return arena_ptr<T>(Make<T>(std::forward<ARGS>(args)...));
	}

	template <class T, class... ARGS>
	unsafe_arena_ptr<T> MakeUnsafePtr(ARGS &&... args) {
		return unsafe_arena_ptr<T>(Make<T>(std::forward<ARGS>(args)...));
	}

	String MakeString(const char *data, const size_t len) {
		data_ptr_t mem = nullptr;

		D_ASSERT(len < NumericLimits<uint32_t>::Maximum());
		const auto size = static_cast<uint32_t>(len);
		if (!String::CanBeInlined(size)) {
			// If the string can't be inlined, we allocate it on the arena allocator
			mem = AllocateAligned(sizeof(char) * size + 1); // +1 for null terminator
			memcpy(mem, data, size);
			mem[size] = '\0';
		}

		return String::Reference(mem ? reinterpret_cast<char *>(mem) : data, size);
	}

	String MakeString(const std::string &data) {
		return MakeString(data.c_str(), data.size());
	}

private:
	void AllocateNewBlock(idx_t min_size);

private:
	//! Internal allocator that is used by the arena allocator
	Allocator &allocator;
	idx_t initial_capacity;
	unsafe_unique_ptr<ArenaChunk> head;
	ArenaChunk *tail;
	//! An allocator wrapper using this arena allocator
	Allocator arena_allocator;
	//! The total allocated size
	idx_t allocated_size = 0;
};

} // namespace goose
