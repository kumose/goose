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

#include <goose/common/common.h>
#include <goose/storage/statistics/base_statistics.h>
#include <goose/common/optional_idx.h>
#include <goose/common/column_index.h>
#include <goose/storage/storage_index.h>

namespace goose {

//! How a table is partitioned by a given set of columns
enum class TablePartitionInfo : uint8_t {
	NOT_PARTITIONED,         // the table is not partitioned by the given set of columns
	SINGLE_VALUE_PARTITIONS, // each partition has exactly one unique value (e.g. bounds = [1,1][2,2][3,3])
	OVERLAPPING_PARTITIONS,  // the partitions overlap **only** at the boundaries (e.g. bounds = [1,2][2,3][3,4]
	DISJOINT_PARTITIONS      // the partitions are disjoint (e.g. bounds = [1,2][3,4][5,6])
};

enum class CountType { COUNT_EXACT, COUNT_APPROXIMATE };

struct PartitionRowGroup {
	virtual ~PartitionRowGroup() = default;
	virtual unique_ptr<BaseStatistics> GetColumnStatistics(const StorageIndex &storage_index) = 0;
	virtual bool MinMaxIsExact(const BaseStatistics &stats, const StorageIndex &storage_index) = 0;
};

struct PartitionStatistics {
	PartitionStatistics();

	//! The row id start
	optional_idx row_start;
	//! The amount of rows in the partition
	idx_t count;
	//! Whether or not the count is exact or approximate
	CountType count_type;
	//! Optional accessor for row group statistics
	shared_ptr<PartitionRowGroup> partition_row_group;
};

} // namespace goose
