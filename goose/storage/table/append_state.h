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
#include <goose/common/vector.h>
#include <goose/function/compression_function.h>
#include <goose/planner/bound_constraint.h>
#include <goose/storage/buffer/buffer_handle.h>
#include <goose/storage/storage_lock.h>
#include <goose/storage/table/table_statistics.h>
#include <goose/transaction/transaction_data.h>

namespace goose {
class ColumnSegment;
class DataTable;
class LocalTableStorage;
class RowGroup;
class UpdateSegment;
class TableCatalogEntry;
template <class T>
struct SegmentNode;
class RowGroupSegmentTree;
class CheckpointLock;

struct TableAppendState;

struct ColumnAppendState {
	//! The current segment of the append
	optional_ptr<SegmentNode<ColumnSegment>> current;
	//! Child append states
	vector<ColumnAppendState> child_appends;
	//! The write lock that is held by the append
	unique_ptr<StorageLockKey> lock;
	//! The compression append state
	unique_ptr<CompressionAppendState> append_state;
};

struct RowGroupAppendState {
	explicit RowGroupAppendState(TableAppendState &parent_p) : parent(parent_p) {
	}

	//! The parent append state
	TableAppendState &parent;
	//! The current row_group we are appending to
	RowGroup *row_group;
	//! The column append states
	unsafe_unique_array<ColumnAppendState> states;
	//! Offset within the row_group
	idx_t offset_in_row_group;
};

struct IndexLock {
	unique_lock<mutex> index_lock;
};

struct TableAppendState {
	TableAppendState();
	~TableAppendState();

	RowGroupAppendState row_group_append_state;
	unique_lock<mutex> append_lock;
	shared_ptr<CheckpointLock> table_lock;
	row_t row_start;
	row_t current_row;
	//! The total number of rows appended by the append operation
	idx_t total_append_count;
	idx_t row_group_start;
	//! The row group segment tree we are appending to
	shared_ptr<RowGroupSegmentTree> row_groups;
	//! The first row-group that has been appended to
	optional_ptr<SegmentNode<RowGroup>> start_row_group;
	//! The transaction data
	TransactionData transaction;
	//! Table statistics
	TableStatistics stats;
	//! Cached hash vector
	Vector hashes;
};

struct ConstraintState {
	explicit ConstraintState(TableCatalogEntry &table_p, const vector<unique_ptr<BoundConstraint>> &bound_constraints)
	    : table(table_p), bound_constraints(bound_constraints) {
	}

	TableCatalogEntry &table;
	const vector<unique_ptr<BoundConstraint>> &bound_constraints;
};

struct LocalAppendState {
	TableAppendState append_state;
	LocalTableStorage *storage;
	unique_ptr<ConstraintState> constraint_state;
};

} // namespace goose
