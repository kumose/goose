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

#include <goose/storage/checkpoint/row_group_writer.h>
#include <goose/storage/checkpoint/table_data_writer.h>
#include <goose/storage/checkpoint_manager.h>

namespace goose {

class InMemoryCheckpointer final : public CheckpointWriter {
public:
	InMemoryCheckpointer(QueryContext context, AttachedDatabase &db, BlockManager &block_manager,
	                     StorageManager &storage_manager, CheckpointOptions options);

	void CreateCheckpoint() override;

	MetadataWriter &GetMetadataWriter() override;
	MetadataManager &GetMetadataManager() override;
	unique_ptr<TableDataWriter> GetTableDataWriter(TableCatalogEntry &table) override;
	optional_ptr<ClientContext> GetClientContext() const {
		return context;
	}
	CheckpointOptions GetCheckpointOptions() const {
		return options;
	}
	PartialBlockManager &GetPartialBlockManager() {
		return partial_block_manager;
	}

public:
	void WriteTable(TableCatalogEntry &table, Serializer &serializer) override;

private:
	optional_ptr<ClientContext> context;
	PartialBlockManager partial_block_manager;
	StorageManager &storage_manager;
	CheckpointOptions options;
};

class InMemoryTableDataWriter : public TableDataWriter {
public:
	InMemoryTableDataWriter(InMemoryCheckpointer &checkpoint_manager, TableCatalogEntry &table);

public:
	void WriteUnchangedTable(MetaBlockPointer pointer, idx_t total_rows) override;
	void FinalizeTable(const TableStatistics &global_stats, DataTableInfo &info, RowGroupCollection &collection,
	                   Serializer &serializer) override;
	unique_ptr<RowGroupWriter> GetRowGroupWriter(RowGroup &row_group) override;
	void FlushPartialBlocks() override;
	CheckpointOptions GetCheckpointOptions() const override;
	MetadataManager &GetMetadataManager() override;

private:
	InMemoryCheckpointer &checkpoint_manager;
};

class InMemoryRowGroupWriter : public RowGroupWriter {
public:
	InMemoryRowGroupWriter(TableCatalogEntry &table, PartialBlockManager &partial_block_manager,
	                       InMemoryCheckpointer &checkpoint_manager);

public:
	CheckpointOptions GetCheckpointOptions() const override;
	WriteStream &GetPayloadWriter() override;
	MetaBlockPointer GetMetaBlockPointer() override;
	optional_ptr<MetadataManager> GetMetadataManager() override;

private:
	//! Underlying writer object
	InMemoryCheckpointer &checkpoint_manager;
	// Nop metadata writer
	MemoryStream metadata_writer;
};

struct InMemoryPartialBlock : public PartialBlock {
public:
	InMemoryPartialBlock(ColumnData &data, ColumnSegment &segment, PartialBlockState state,
	                     BlockManager &block_manager);
	~InMemoryPartialBlock() override;

public:
	void Flush(QueryContext context, const idx_t free_space_left) override;
	void Merge(PartialBlock &other, idx_t offset, idx_t other_size) override;
	void AddSegmentToTail(ColumnData &data, ColumnSegment &segment, uint32_t offset_in_block) override;
	void Clear() override;
};
} // namespace goose
