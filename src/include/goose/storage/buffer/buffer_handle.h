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
