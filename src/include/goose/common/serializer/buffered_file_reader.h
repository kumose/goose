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
