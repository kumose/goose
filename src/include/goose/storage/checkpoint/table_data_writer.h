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

#include <goose/parallel/task_executor.h>
#include <goose/storage/checkpoint/row_group_writer.h>

namespace goose {
class GooseTableEntry;
class TableStatistics;

//! The table data writer is responsible for writing the data of a table to storage.
//
//! This is meant to encapsulate and abstract:
//!  - Storage/encoding of table metadata (block pointers)
//!  - Mapping management of data block locations
//! Abstraction will support, for example: tiering, versioning, or splitting into multiple block managers.
class TableDataWriter {
public:
	explicit TableDataWriter(TableCatalogEntry &table, QueryContext context);
	virtual ~TableDataWriter();

public:
	void WriteTableData(Serializer &metadata_serializer);

	virtual void WriteUnchangedTable(MetaBlockPointer pointer, idx_t total_rows) = 0;
	virtual void FinalizeTable(const TableStatistics &global_stats, DataTableInfo &info, RowGroupCollection &collection,
	                           Serializer &serializer) = 0;
	virtual unique_ptr<RowGroupWriter> GetRowGroupWriter(RowGroup &row_group) = 0;

	virtual void AddRowGroup(RowGroupPointer &&row_group_pointer, unique_ptr<RowGroupWriter> writer);
	virtual CheckpointOptions GetCheckpointOptions() const = 0;
	virtual void FlushPartialBlocks() = 0;
	virtual MetadataManager &GetMetadataManager() = 0;
	bool CanOverrideBaseStats() const {
		return override_base_stats;
	}
	void SetCannotOverrideStats() {
		override_base_stats = false;
	}

	DatabaseInstance &GetDatabase();
	unique_ptr<TaskExecutor> CreateTaskExecutor();

protected:
	GooseTableEntry &table;
	optional_ptr<ClientContext> context;
	//! Pointers to the start of each row group.
	vector<RowGroupPointer> row_group_pointers;
	bool override_base_stats = true;
};

class SingleFileTableDataWriter : public TableDataWriter {
public:
	SingleFileTableDataWriter(SingleFileCheckpointWriter &checkpoint_manager, TableCatalogEntry &table,
	                          MetadataWriter &table_data_writer);

public:
	void WriteUnchangedTable(MetaBlockPointer pointer, idx_t total_rows) override;
	void FinalizeTable(const TableStatistics &global_stats, DataTableInfo &info, RowGroupCollection &collection,
	                   Serializer &serializer) override;
	unique_ptr<RowGroupWriter> GetRowGroupWriter(RowGroup &row_group) override;
	CheckpointOptions GetCheckpointOptions() const override;
	void FlushPartialBlocks() override;
	MetadataManager &GetMetadataManager() override;

private:
	SingleFileCheckpointWriter &checkpoint_manager;
	//! Writes the actual table data
	MetadataWriter &table_data_writer;
	//! The root pointer, if we are re-using metadata of the table
	MetaBlockPointer existing_pointer;
	optional_idx existing_rows;
	vector<MetaBlockPointer> existing_pointers;
};

} // namespace goose
