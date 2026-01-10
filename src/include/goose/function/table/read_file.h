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

#include <goose/common/multi_file/multi_file_function.h>
#include <goose/common/serializer/memory_stream.h>
#include <goose/utility/utf8proc_wrapper.h>

namespace goose {

struct ReadFileBindData : public TableFunctionData {
	unique_ptr<BaseFileReaderOptions> options;

	static constexpr const idx_t FILE_NAME_COLUMN = 0;
	static constexpr const idx_t FILE_CONTENT_COLUMN = 1;
	static constexpr const idx_t FILE_SIZE_COLUMN = 2;
	static constexpr const idx_t FILE_LAST_MODIFIED_COLUMN = 3;
};

struct ReadFileGlobalState : public GlobalTableFunctionState {
	ReadFileGlobalState() {
	}

	shared_ptr<MultiFileList> file_list;
	vector<idx_t> column_ids;
	bool requires_file_open = false;

	unique_ptr<MemoryStream> stream;
};

} // namespace goose
