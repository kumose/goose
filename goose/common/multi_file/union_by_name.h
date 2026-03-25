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

#include <goose/common/types.h>
#include <goose/common/vector.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/common/helper.h>
#include <goose/parallel/task_executor.h>
#include <goose/common/multi_file/base_file_reader.h>
#include <goose/common/multi_file/multi_file_options.h>

namespace goose {
struct MultiFileReader;
struct MultiFileReaderInterface;

class UnionByName {
public:
	static void CombineUnionTypes(const vector<string> &new_names, const vector<LogicalType> &new_types,
	                              vector<LogicalType> &union_col_types, vector<string> &union_col_names,
	                              case_insensitive_map_t<idx_t> &union_names_map);

	//! Union all files(readers) by their col names
	static vector<shared_ptr<BaseUnionData>>
	UnionCols(ClientContext &context, const vector<OpenFileInfo> &files, vector<LogicalType> &union_col_types,
	          vector<string> &union_col_names, BaseFileReaderOptions &options, MultiFileOptions &file_options,
	          MultiFileReader &multi_file_reader, MultiFileReaderInterface &interface);
};

} // namespace goose
