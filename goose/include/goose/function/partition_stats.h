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
