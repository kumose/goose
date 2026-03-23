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
