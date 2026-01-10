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

#include <goose/storage/partial_block_manager.h>
#include <goose/catalog/catalog_entry/index_catalog_entry.h>
#include <goose/catalog/catalog.h>

namespace goose {
class DatabaseInstance;
class ClientContext;
class ColumnSegment;
class MetadataReader;
class SchemaCatalogEntry;
class SequenceCatalogEntry;
class TableCatalogEntry;
class ViewCatalogEntry;
class TypeCatalogEntry;

class CheckpointWriter {
public:
	explicit CheckpointWriter(AttachedDatabase &db) : db(db) {
	}
	virtual ~CheckpointWriter() {
	}

	//! The database
	AttachedDatabase &db;

	virtual void CreateCheckpoint() = 0;
	virtual MetadataManager &GetMetadataManager() = 0;
	virtual MetadataWriter &GetMetadataWriter() = 0;
	virtual unique_ptr<TableDataWriter> GetTableDataWriter(TableCatalogEntry &table) = 0;

protected:
	virtual void WriteEntry(CatalogEntry &entry, Serializer &serializer);
	virtual void WriteSchema(SchemaCatalogEntry &schema, Serializer &serializer);
	virtual void WriteTable(TableCatalogEntry &table, Serializer &serializer) = 0;
	virtual void WriteView(ViewCatalogEntry &table, Serializer &serializer);
	virtual void WriteSequence(SequenceCatalogEntry &table, Serializer &serializer);
	virtual void WriteMacro(ScalarMacroCatalogEntry &table, Serializer &serializer);
	virtual void WriteTableMacro(TableMacroCatalogEntry &table, Serializer &serializer);
	virtual void WriteIndex(IndexCatalogEntry &index_catalog_entry, Serializer &serializer);
	virtual void WriteType(TypeCatalogEntry &type, Serializer &serializer);
};

class CheckpointReader {
public:
	explicit CheckpointReader(Catalog &catalog) : catalog(catalog) {
	}
	virtual ~CheckpointReader() {
	}

protected:
	Catalog &catalog;

protected:
	virtual void LoadCheckpoint(CatalogTransaction transaction, MetadataReader &reader);
	virtual void ReadEntry(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadSchema(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadTable(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadView(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadSequence(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadMacro(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadTableMacro(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadIndex(CatalogTransaction transaction, Deserializer &deserializer);
	virtual void ReadType(CatalogTransaction transaction, Deserializer &deserializer);

	virtual void ReadTableData(CatalogTransaction transaction, Deserializer &deserializer,
	                           BoundCreateTableInfo &bound_info);
};

class SingleFileCheckpointReader final : public CheckpointReader {
public:
	explicit SingleFileCheckpointReader(SingleFileStorageManager &storage)
	    : CheckpointReader(Catalog::GetCatalog(storage.GetAttached())), storage(storage) {
	}

	void LoadFromStorage();
	MetadataManager &GetMetadataManager();

	//! The database
	SingleFileStorageManager &storage;
};

//! CheckpointWriter is responsible for checkpointing the database
class SingleFileRowGroupWriter;
class SingleFileTableDataWriter;

class SingleFileCheckpointWriter final : public CheckpointWriter {
	friend class SingleFileRowGroupWriter;
	friend class SingleFileTableDataWriter;

public:
	SingleFileCheckpointWriter(QueryContext context, AttachedDatabase &db, BlockManager &block_manager,
	                           CheckpointOptions options);

	void CreateCheckpoint() override;

	MetadataWriter &GetMetadataWriter() override;
	MetadataManager &GetMetadataManager() override;
	unique_ptr<TableDataWriter> GetTableDataWriter(TableCatalogEntry &table) override;

	BlockManager &GetBlockManager();
	CheckpointOptions GetCheckpointOptions() const {
		return options;
	}
	optional_ptr<ClientContext> GetClientContext() const {
		return context;
	}

public:
	void WriteTable(TableCatalogEntry &table, Serializer &serializer) override;

private:
	optional_ptr<ClientContext> context;
	//! The metadata writer is responsible for writing schema information
	unique_ptr<MetadataWriter> metadata_writer;
	//! The table data writer is responsible for writing the DataPointers used by the table chunks
	unique_ptr<MetadataWriter> table_metadata_writer;
	//! Because this is single-file storage, we can share partial blocks across
	//! an entire checkpoint.
	PartialBlockManager partial_block_manager;
	//! Checkpoint type
	CheckpointOptions options;
	//! Block usage count for verification purposes
	unordered_map<block_id_t, idx_t> verify_block_usage_count;
};

} // namespace goose
