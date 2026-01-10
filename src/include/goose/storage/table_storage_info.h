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
