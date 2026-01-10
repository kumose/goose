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

#include <goose/common/types-import.h>
#include <goose/common/common.h>
#include <goose/storage/table/table_index_list.h>
#include <goose/storage/storage_lock.h>

namespace goose {
class DatabaseInstance;
class TableIOManager;

struct DataTableInfo {
	friend class DataTable;

public:
	DataTableInfo(AttachedDatabase &db, shared_ptr<TableIOManager> table_io_manager_p, string schema, string table);

	//! Bind unknown indexes throwing an exception if binding fails.
	//! Only binds the specified index type, or all, if nullptr.
	void BindIndexes(ClientContext &context, const char *index_type = nullptr);

	//! Whether or not the table is temporary
	bool IsTemporary() const;

	AttachedDatabase &GetDB() {
		return db;
	}

	TableIOManager &GetIOManager() {
		return *table_io_manager;
	}

	TableIndexList &GetIndexes() {
		return indexes;
	}
	const vector<IndexStorageInfo> &GetIndexStorageInfo() const {
		return index_storage_infos;
	}
	unique_ptr<StorageLockKey> GetSharedLock() {
		return checkpoint_lock.GetSharedLock();
	}
	bool AppendRequiresNewRowGroup(RowGroupCollection &collection, transaction_t checkpoint_id);
	void VerifyIndexBuffers();

	string GetSchemaName();
	string GetTableName();
	void SetTableName(string name);

private:
	//! The database instance of the table
	AttachedDatabase &db;
	//! The table IO manager
	shared_ptr<TableIOManager> table_io_manager;
	//! Lock for modifying the name
	mutex name_lock;
	//! The schema of the table
	string schema;
	//! The name of the table
	string table;
	//! The physical list of indexes of this table
	TableIndexList indexes;
	//! Index storage information of the indexes created by this table
	vector<IndexStorageInfo> index_storage_infos;
	//! Lock held while checkpointing
	StorageLock checkpoint_lock;
	//! The last seen checkpoint while doing a concurrent operation, if any
	optional_idx last_seen_checkpoint;
	//! The amount of row groups the checkpoint is processing
	optional_idx checkpoint_row_group_count;
};

} // namespace goose
