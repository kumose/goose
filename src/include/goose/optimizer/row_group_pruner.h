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
