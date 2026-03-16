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
#include <goose/common/file_system.h>

namespace goose {

#define FILE_BUFFER_SIZE 4096

class BufferedFileWriter : public WriteStream {
public:
	static constexpr FileOpenFlags DEFAULT_OPEN_FLAGS = FileFlags::FILE_FLAGS_WRITE | FileFlags::FILE_FLAGS_FILE_CREATE;

	//! Serializes to a buffer allocated by the serializer, will expand when
	//! writing past the initial threshold
	GOOSE_API BufferedFileWriter(FileSystem &fs, const string &path, FileOpenFlags open_flags = DEFAULT_OPEN_FLAGS);

	FileSystem &fs;
	string path;
	unsafe_unique_array<data_t> data;
	idx_t offset;
	idx_t total_written;
	unique_ptr<FileHandle> handle;

public:
	GOOSE_API void WriteData(const_data_ptr_t buffer, idx_t write_size) override;
	//! Flush all changes to the file and then close the file
	GOOSE_API void Close();
	//! Flush all changes and fsync the file to disk
	GOOSE_API void Sync();
	//! Flush the buffer to the file (without sync)
	GOOSE_API void Flush();
	//! Returns the current size of the file
	GOOSE_API idx_t GetFileSize();
	//! Truncate the size to a previous size (given that size <= GetFileSize())
	GOOSE_API void Truncate(idx_t size);

	GOOSE_API idx_t GetTotalWritten() const;
};

} // namespace goose
