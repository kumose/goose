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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/storage/data_pointer.h>
#include <goose/storage/table/table_statistics.h>
#include <goose/storage/metadata/metadata_manager.h>

namespace goose {
class BaseStatistics;

class PersistentTableData {
public:
	explicit PersistentTableData(idx_t column_count);
	~PersistentTableData();

	MetaBlockPointer base_table_pointer;
	TableStatistics table_stats;
	idx_t total_rows;
	idx_t row_group_count;
	MetaBlockPointer block_pointer;
};

} // namespace goose
