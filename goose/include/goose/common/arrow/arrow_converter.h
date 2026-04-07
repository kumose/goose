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
