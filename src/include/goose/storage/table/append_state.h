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
