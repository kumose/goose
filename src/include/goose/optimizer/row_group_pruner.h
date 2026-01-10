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

#include <goose/function/table_function.h>
#include <goose/main/client_context.h>
#include <goose/planner/operator/logical_order.h>
#include <goose/storage/table/scan_state.h>

namespace goose {
class LogicalGet;
class LogicalOperator;

class RowGroupPruner {
public:
	explicit RowGroupPruner(ClientContext &context);

	//! Reorder and try to prune row groups in queries with LIMIT or simple aggregates
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);
	//! Whether we can perform the optimization on this operator
	bool TryOptimize(LogicalOperator &op) const;

private:
	ClientContext &context;

private:
	void GetLimitAndOffset(const LogicalLimit &logical_limit, optional_idx &row_limit, optional_idx &row_offset) const;
	optional_ptr<LogicalOrder> FindLogicalOrder(const LogicalLimit &logical_limit) const;
	optional_ptr<LogicalGet> FindLogicalGet(const LogicalOrder &logical_order, ColumnIndex &column_index) const;
	// row_limit, row_offset, primary_order, logical_get, logical_limit
	unique_ptr<RowGroupOrderOptions> CreateRowGroupReordererOptions(optional_idx row_limit, optional_idx row_offset,
	                                                                const BoundOrderByNode &primary_order,
	                                                                const LogicalGet &logical_get,
	                                                                const StorageIndex &storage_index,
	                                                                LogicalLimit &logical_limit) const;
};

} // namespace goose
