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
#include <goose/common/file_system.h>

namespace goose {
class CompressedFile;

struct StreamData {
	// various buffers & pointers
	bool write = false;
	bool refresh = false;
	unsafe_unique_array<data_t> in_buff;
	unsafe_unique_array<data_t> out_buff;
	data_ptr_t out_buff_start = nullptr;
	data_ptr_t out_buff_end = nullptr;
	data_ptr_t in_buff_start = nullptr;
	data_ptr_t in_buff_end = nullptr;

	idx_t in_buf_size = 0;
	idx_t out_buf_size = 0;
};

struct StreamWrapper {
	GOOSE_API virtual ~StreamWrapper();

	GOOSE_API virtual void Initialize(QueryContext context, CompressedFile &file, bool write) = 0;
	GOOSE_API virtual bool Read(StreamData &stream_data) = 0;
	GOOSE_API virtual void Write(CompressedFile &file, StreamData &stream_data, data_ptr_t buffer,
	                              int64_t nr_bytes) = 0;
	GOOSE_API virtual void Close() = 0;
};

class CompressedFileSystem : public FileSystem {
public:
	GOOSE_API int64_t Read(FileHandle &handle, void *buffer, int64_t nr_bytes) override;
	GOOSE_API int64_t Write(FileHandle &handle, void *buffer, int64_t nr_bytes) override;

	GOOSE_API void Reset(FileHandle &handle) override;

	GOOSE_API int64_t GetFileSize(FileHandle &handle) override;

	GOOSE_API bool OnDiskFile(FileHandle &handle) override;
	GOOSE_API bool CanSeek() override;

	GOOSE_API virtual unique_ptr<StreamWrapper> CreateStream() = 0;
	GOOSE_API virtual idx_t InBufferSize() = 0;
	GOOSE_API virtual idx_t OutBufferSize() = 0;
};

class CompressedFile : public FileHandle {
public:
	GOOSE_API CompressedFile(CompressedFileSystem &fs, unique_ptr<FileHandle> child_handle_p, const string &path);
	GOOSE_API ~CompressedFile() override;

	GOOSE_API idx_t GetProgress() override;

	CompressedFileSystem &compressed_fs;
	unique_ptr<FileHandle> child_handle;
	//! Whether the file is opened for reading or for writing
	bool write = false;
	StreamData stream_data;

public:
	GOOSE_API void Initialize(QueryContext context, bool write);
	GOOSE_API int64_t ReadData(void *buffer, int64_t nr_bytes);
	GOOSE_API int64_t WriteData(data_ptr_t buffer, int64_t nr_bytes);
	GOOSE_API void Close() override;

private:
	idx_t current_position = 0;
	unique_ptr<StreamWrapper> stream_wrapper;
};

} // namespace goose
