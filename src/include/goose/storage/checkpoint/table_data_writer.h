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
