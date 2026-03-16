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

#include <goose/function/partition_stats.h>
#include <goose/storage/statistics/base_statistics.h>
#include <goose/storage/table/row_group.h>
#include <goose/storage/table/row_group_segment_tree.h>
#include <goose/storage/table/segment_tree.h>

namespace goose {

enum class OrderByStatistics { MIN, MAX };
enum class RowGroupOrderType { ASC, DESC };
enum class OrderByColumnType { NUMERIC, STRING };

struct RowGroupOrderOptions {
	RowGroupOrderOptions(const StorageIndex &column_idx_p, OrderByStatistics order_by_p, RowGroupOrderType order_type_p,
	                     OrderByColumnType column_type_p, optional_idx row_limit_p = optional_idx(),
	                     idx_t row_group_offset_p = 0)
	    : column_idx(column_idx_p), order_by(order_by_p), order_type(order_type_p), column_type(column_type_p),
	      row_limit(row_limit_p), row_group_offset(row_group_offset_p) {
	}

	const StorageIndex column_idx;
	const OrderByStatistics order_by;
	const RowGroupOrderType order_type;
	const OrderByColumnType column_type;
	const optional_idx row_limit;
	const idx_t row_group_offset;
};

struct OffsetPruningResult {
	idx_t offset_remainder;
	idx_t pruned_row_group_count;
};

class RowGroupReorderer {
public:
	explicit RowGroupReorderer(const RowGroupOrderOptions &options_p);
	optional_ptr<SegmentNode<RowGroup>> GetRootSegment(RowGroupSegmentTree &row_groups);
	optional_ptr<SegmentNode<RowGroup>> GetNextRowGroup(SegmentNode<RowGroup> &row_group);

	static Value RetrieveStat(const BaseStatistics &stats, OrderByStatistics order_by, OrderByColumnType column_type);
	static OffsetPruningResult GetOffsetAfterPruning(OrderByStatistics order_by, OrderByColumnType column_type,
	                                                 RowGroupOrderType order_type, const StorageIndex &column_idx,
	                                                 idx_t row_offset, vector<PartitionStatistics> &stats);

private:
	const RowGroupOrderOptions options;

	idx_t offset;
	bool initialized;
	vector<reference<SegmentNode<RowGroup>>> ordered_row_groups;
};

} // namespace goose
