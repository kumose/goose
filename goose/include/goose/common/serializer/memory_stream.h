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

#include <goose/common/serializer/write_stream.h>
#include <goose/common/serializer/read_stream.h>
#include <goose/common/typedefs.h>

namespace goose {
class Allocator;

class MemoryStream : public WriteStream, public ReadStream {
private:
	optional_ptr<Allocator> allocator;
	idx_t position;
	idx_t capacity;
	data_ptr_t data;

public:
	static constexpr idx_t DEFAULT_INITIAL_CAPACITY = 512;

	//! Create a new owning MemoryStream with an internal  backing buffer with the specified capacity. The stream will
	//! own the backing buffer, resize it when needed and free its memory when the stream is destroyed
	explicit MemoryStream(Allocator &allocator, idx_t capacity = DEFAULT_INITIAL_CAPACITY);

	//! Create a new owning MemoryStream with an internal  backing buffer with the specified capacity. The stream will
	//! own the backing buffer, resize it when needed and free its memory when the stream is destroyed
	explicit MemoryStream(idx_t capacity = DEFAULT_INITIAL_CAPACITY);

	//! Create a new non-owning MemoryStream over the specified external buffer and capacity. The stream will not take
	//! ownership of the backing buffer, will not attempt to resize it and will not free the memory when the stream
	//! is destroyed
	explicit MemoryStream(data_ptr_t buffer, idx_t capacity);

	//! Cant copy!
	MemoryStream(const MemoryStream &) = delete;
	MemoryStream &operator=(const MemoryStream &) = delete;

	MemoryStream(MemoryStream &&other) noexcept;
	MemoryStream &operator=(MemoryStream &&other) noexcept;

	~MemoryStream() override;

	//! Write data to the stream.
	//! Throws if the write would exceed the capacity of the stream and the backing buffer is not owned by the stream
	void WriteData(const_data_ptr_t buffer, idx_t write_size) override;
	void GrowCapacity(idx_t write_size);

	//! Read data from the stream.
	//! Throws if the read would exceed the capacity of the stream
	void ReadData(data_ptr_t buffer, idx_t read_size) override;
	void ReadData(QueryContext context, data_ptr_t buffer, idx_t read_size) override;

	//! Rewind the stream to the start, keeping the capacity and the backing buffer intact
	void Rewind();

	//! Release ownership of the backing buffer and turn a owning stream into a non-owning one.
	//! The stream will no longer be responsible for freeing the data.
	//! The stream will also no longer attempt to automatically resize the buffer when the capacity is reached.
	void Release();

	//! Get a pointer to the underlying backing buffer
	data_ptr_t GetData() const;

	//! Get the current position in the stream
	idx_t GetPosition() const;

	//! Get the capacity of the stream
	idx_t GetCapacity() const;

	//! Set the position in the stream
	void SetPosition(idx_t position);
};

} // namespace goose
