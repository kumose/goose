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
