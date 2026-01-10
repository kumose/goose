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

#include <goose/storage/table/row_group_collection.h>

namespace goose {
class PartialBlockManager;

struct OptimisticWriteCollection {
	~OptimisticWriteCollection();

	shared_ptr<RowGroupCollection> collection;
	idx_t last_flushed = 0;
	idx_t complete_row_groups = 0;
	vector<unique_ptr<PartialBlockManager>> partial_block_managers;
};

enum class OptimisticWritePartialManagers { PER_COLUMN, GLOBAL };

class OptimisticDataWriter {
public:
	OptimisticDataWriter(ClientContext &context, DataTable &table);
	OptimisticDataWriter(DataTable &table, OptimisticDataWriter &parent);
	~OptimisticDataWriter();

	//! Creates a collection to write to
	unique_ptr<OptimisticWriteCollection>
	CreateCollection(DataTable &storage, const vector<LogicalType> &insert_types,
	                 OptimisticWritePartialManagers type = OptimisticWritePartialManagers::PER_COLUMN);
	//! Write a new row group to disk (if possible)
	void WriteNewRowGroup(OptimisticWriteCollection &row_groups);
	//! Write the last row group of a collection to disk
	void WriteLastRowGroup(OptimisticWriteCollection &row_groups);
	//! Final flush of the optimistic writer - fully flushes the partial block manager
	void FinalFlush();
	//! Merge the partially written blocks from one optimistic writer into another
	void Merge(OptimisticDataWriter &other);
	void Merge(unique_ptr<PartialBlockManager> &other_manager);
	//! Rollback
	void Rollback();

	//! Return the client context.
	ClientContext &GetClientContext() {
		return context;
	}

private:
	//! Prepare a write to disk
	bool PrepareWrite();
	//! Flushes a specific row group to disk
	void FlushToDisk(OptimisticWriteCollection &collection, const vector<const_reference<RowGroup>> &row_groups,
	                 const vector<int64_t> &segment_indexes);

private:
	//! The client context in which we're writing the data.
	ClientContext &context;
	//! The table.
	DataTable &table;
	//! The partial block manager, if any.
	unique_ptr<PartialBlockManager> partial_manager;
};

} // namespace goose
