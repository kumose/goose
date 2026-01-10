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
