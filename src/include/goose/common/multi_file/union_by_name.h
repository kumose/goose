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
