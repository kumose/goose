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
#include <goose/common/types/value.h>
#include <goose/storage/arena_allocator.h>

namespace goose {
//! A string heap is the owner of a set of strings, strings can be inserted into
//! it On every insert, a pointer to the inserted string is returned The
//! returned pointer will remain valid until the StringHeap is destroyed
class StringHeap {
public:
	GOOSE_API explicit StringHeap(Allocator &allocator = Allocator::DefaultAllocator());

	GOOSE_API void Destroy();
	GOOSE_API void Move(StringHeap &other);

	//! Add a string to the string heap, returns a pointer to the string
	GOOSE_API string_t AddString(const char *data, idx_t len);
	//! Add a string to the string heap, returns a pointer to the string
	GOOSE_API string_t AddString(const char *data);
	//! Add a string to the string heap, returns a pointer to the string
	GOOSE_API string_t AddString(const string &data);
	//! Add a string to the string heap, returns a pointer to the string
	GOOSE_API string_t AddString(const string_t &data);
	//! Add a blob to the string heap; blobs can be non-valid UTF8
	GOOSE_API string_t AddBlob(const string_t &data);
	//! Add a blob to the string heap; blobs can be non-valid UTF8
	GOOSE_API string_t AddBlob(const char *data, idx_t len);
	//! Allocates space for an empty string of size "len" on the heap
	GOOSE_API string_t EmptyString(idx_t len);

	//! Size of strings
	GOOSE_API idx_t SizeInBytes() const;
	//! Total allocation size (cached)
	GOOSE_API idx_t AllocationSize() const;

	GOOSE_API ArenaAllocator &GetAllocator() {
		return allocator;
	}

private:
	ArenaAllocator allocator;
};

} // namespace goose
