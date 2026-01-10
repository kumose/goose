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

namespace goose {

class PipeFileSystem : public FileSystem {
public:
	static unique_ptr<FileHandle> OpenPipe(QueryContext context, unique_ptr<FileHandle> handle);

	int64_t Read(FileHandle &handle, void *buffer, int64_t nr_bytes) override;
	int64_t Write(FileHandle &handle, void *buffer, int64_t nr_bytes) override;

	int64_t GetFileSize(FileHandle &handle) override;
	timestamp_t GetLastModifiedTime(FileHandle &handle) override;

	void Reset(FileHandle &handle) override;
	bool OnDiskFile(FileHandle &handle) override {
		return false;
	};
	bool CanSeek() override {
		return false;
	}
	bool IsPipe(const string &filename, optional_ptr<FileOpener> opener) override {
		return true;
	}
	void FileSync(FileHandle &handle) override;

	std::string GetName() const override {
		return "PipeFileSystem";
	}
};

} // namespace goose
