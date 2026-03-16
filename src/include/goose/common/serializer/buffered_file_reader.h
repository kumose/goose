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

#include <goose/common/serializer/buffered_file_writer.h>
#include <goose/common/serializer/read_stream.h>

namespace goose {

class BufferedFileReader : public ReadStream {
public:
	BufferedFileReader(FileSystem &fs, const char *path, FileLockType lock_type = FileLockType::READ_LOCK,
	                   optional_ptr<FileOpener> opener = nullptr);
	BufferedFileReader(FileSystem &fs, unique_ptr<FileHandle> handle);

	FileSystem &fs;
	unsafe_unique_array<data_t> data;
	idx_t offset;
	idx_t read_data;
	unique_ptr<FileHandle> handle;

public:
	void ReadData(data_ptr_t buffer, uint64_t read_size) override;
	void ReadData(QueryContext context, data_ptr_t buffer, uint64_t read_size) override;

	//! Returns true if the reader has finished reading the entire file
	bool Finished();

	idx_t FileSize() {
		return file_size;
	}

	//! Resets reading - beginning at position 0
	void Reset();
	void Seek(uint64_t location);
	uint64_t CurrentOffset();

private:
	idx_t file_size;
	idx_t total_read;
};

} // namespace goose
