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

namespace goose {
class FileSystem;
class QueryContext;

enum class DataFileType : uint8_t {
	FILE_DOES_NOT_EXIST, // file does not exist
	GOOSE_FILE,         // goose database file
	SQLITE_FILE,         // sqlite database file
	PARQUET_FILE,        // parquet file
	UNKNOWN_FILE         // unknown file type
};

class MagicBytes {
public:
	static DataFileType CheckMagicBytes(QueryContext context, FileSystem &fs, const string &path);
};

} // namespace goose
