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
#include <goose/optimizer/remove_unused_columns.h>
#include <goose/common/table_column.h>

namespace goose {
class LogicalOperator;
class LogicalGet;
class LogicalLimit;
class Optimizer;

//! Transform
class LateMaterialization : public BaseColumnPruner {
public:
	explicit LateMaterialization(Optimizer &optimizer);

	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);

private:
	bool TryLateMaterialization(unique_ptr<LogicalOperator> &op);

	unique_ptr<LogicalGet> ConstructLHS(LogicalGet &get);
	vector<ColumnBinding> ConstructRHS(unique_ptr<LogicalOperator> &op);
	vector<idx_t> GetOrInsertRowIds(LogicalGet &get);

	void ReplaceTopLevelTableIndex(LogicalOperator &op, idx_t new_index);
	void ReplaceTableReferences(unique_ptr<Expression> &expr, idx_t new_table_index);
	unique_ptr<Expression> GetExpression(LogicalOperator &op, idx_t column_index);
	void ReplaceExpressionReferences(LogicalOperator &next_op, unique_ptr<Expression> &expr);
	bool OptimizeLargeLimit(LogicalLimit &limit, idx_t limit_val, bool has_offset);

private:
	Optimizer &optimizer;
	//! The max row count for which we will consider late materialization
	idx_t max_row_count;
	//! The row-id column ids
	vector<column_t> row_id_column_ids;
	//! The row-id columns
	vector<TableColumn> row_id_columns;
};

} // namespace goose
