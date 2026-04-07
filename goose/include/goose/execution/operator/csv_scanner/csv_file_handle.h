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
