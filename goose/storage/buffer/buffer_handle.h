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

#include <goose/storage/storage_info.h>
#include <goose/common/file_buffer.h>

namespace goose {
class BlockHandle;
class FileBuffer;

class BufferHandle {
public:
	GOOSE_API BufferHandle();
	GOOSE_API explicit BufferHandle(shared_ptr<BlockHandle> handle, optional_ptr<FileBuffer> node);
	GOOSE_API ~BufferHandle();
	// disable copy constructors
	BufferHandle(const BufferHandle &other) = delete;
	BufferHandle &operator=(const BufferHandle &) = delete;
	//! enable move constructors
	GOOSE_API BufferHandle(BufferHandle &&other) noexcept;
	GOOSE_API BufferHandle &operator=(BufferHandle &&) noexcept;

public:
	//! Returns whether or not the BufferHandle is valid.
	GOOSE_API bool IsValid() const;
	//! Returns a pointer to the buffer data. Handle must be valid.
	inline data_ptr_t Ptr() const {
		D_ASSERT(IsValid());
		return node->buffer;
	}
	//! Returns a pointer to the buffer data. Handle must be valid.
	inline data_ptr_t Ptr() {
		D_ASSERT(IsValid());
		return node->buffer;
	}
	//! Gets the underlying file buffer. Handle must be valid.
	GOOSE_API FileBuffer &GetFileBuffer();
	//! Destroys the buffer handle
	GOOSE_API void Destroy();

	const shared_ptr<BlockHandle> &GetBlockHandle() const {
		return handle;
	}

private:
	//! The block handle
	shared_ptr<BlockHandle> handle;
	//! The managed buffer node
	optional_ptr<FileBuffer> node;
};

} // namespace goose
