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

#include <goose/common/file_system.h>
#include <goose/common/types-import.h>
#include <goose/common/helper.h>
#include <goose/common/allocator.h>
#include <goose/execution/operator/csv_scanner/encode/csv_encoder.h>
#include <goose/main/client_context.h>

namespace goose {
class Allocator;
class FileSystem;
struct CSVReaderOptions;

class CSVFileHandle {
public:
	CSVFileHandle(ClientContext &context, unique_ptr<FileHandle> file_handle_p, const OpenFileInfo &file,
	              const CSVReaderOptions &options);

	mutex main_mutex;

	bool CanSeek() const;
	void Seek(idx_t position) const;
	bool OnDiskFile() const;
	bool IsPipe() const;

	void Reset();

	idx_t FileSize() const;

	bool FinishedReading() const;

	idx_t Read(void *buffer, idx_t nr_bytes);

	string ReadLine();

	string GetFilePath();

	static unique_ptr<FileHandle> OpenFileHandle(FileSystem &fs, Allocator &allocator, const OpenFileInfo &file,
	                                             FileCompressionType compression);
	static unique_ptr<CSVFileHandle> OpenFile(ClientContext &context, const OpenFileInfo &file,
	                                          const CSVReaderOptions &options);
	FileCompressionType compression_type;

	double GetProgress() const;

private:
	QueryContext context;
	unique_ptr<FileHandle> file_handle;
	CSVEncoder encoder;
	const OpenFileInfo file;
	bool can_seek = false;
	bool on_disk_file = false;
	bool is_pipe = false;
	idx_t uncompressed_bytes_read = 0;

	idx_t file_size = 0;

	idx_t requested_bytes = 0;
	//! If we finished reading the file
	bool finished = false;
};

} // namespace goose
