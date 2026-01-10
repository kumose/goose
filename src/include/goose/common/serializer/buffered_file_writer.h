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
