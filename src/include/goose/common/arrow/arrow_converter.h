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

#include <goose/common/types/data_chunk.h>
#include <goose/common/arrow/arrow.h>
#include <goose/main/client_properties.h>
#include <list>

namespace goose {
class ArrowTypeExtensionData;
struct DBConfig;
struct ArrowConverter {
	GOOSE_API static void ToArrowSchema(ArrowSchema *out_schema, const vector<LogicalType> &types,
	                                     const vector<string> &names, ClientProperties &options);
	GOOSE_API static void
	ToArrowArray(DataChunk &input, ArrowArray *out_array, ClientProperties options,
	             const unordered_map<idx_t, const shared_ptr<ArrowTypeExtensionData>> &extension_type_cast);
};

//===--------------------------------------------------------------------===//
// Arrow Schema
//===--------------------------------------------------------------------===//
struct GooseArrowSchemaHolder {
	// unused in children
	vector<ArrowSchema> children;
	// unused in children
	vector<ArrowSchema *> children_ptrs;
	//! used for nested structures
	std::list<vector<ArrowSchema>> nested_children;
	std::list<vector<ArrowSchema *>> nested_children_ptr;
	//! This holds strings created to represent decimal types
	vector<unsafe_unique_array<char>> owned_type_names;
	vector<unsafe_unique_array<char>> owned_column_names;
	//! This holds any values created for metadata info
	vector<unsafe_unique_array<char>> metadata_info;
	vector<unsafe_unique_array<char>> extension_format;
};

} // namespace goose
