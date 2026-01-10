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
