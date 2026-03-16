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

#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/storage/block.h>
#include <goose/storage/index_storage_info.h>
#include <goose/storage/storage_info.h>
#include <goose/storage/table/column_data.h>
#include <goose/common/optional_idx.h>

namespace goose {

//! Column segment information
struct ColumnSegmentInfo {
	idx_t row_group_index;
	idx_t column_id;
	string column_path;
	idx_t segment_idx;
	string segment_type;
	idx_t segment_start;
	idx_t segment_count;
	string compression_type;
	string segment_stats;
	bool has_updates;
	bool persistent;
	block_id_t block_id;
	vector<block_id_t> additional_blocks;
	idx_t block_offset;
	string segment_info;
};

//! Table storage information
class TableStorageInfo {
public:
	//! The (estimated) cardinality of the table
	optional_idx cardinality;
	//! Info of the indexes of a table
	vector<IndexInfo> index_info;
};

} // namespace goose
