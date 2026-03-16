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
