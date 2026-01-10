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

#include <goose/catalog/catalog_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/catalog/entry_lookup_info.h>

namespace goose {
class ClientContext;

class StandardEntry;
class TableCatalogEntry;
class TableFunctionCatalogEntry;
class SequenceCatalogEntry;

enum class OnCreateConflict : uint8_t;

struct AlterTableInfo;
struct CreateIndexInfo;
struct CreateFunctionInfo;
struct CreateCollationInfo;
struct CreateViewInfo;
struct BoundCreateTableInfo;
struct CreatePragmaFunctionInfo;
struct CreateSequenceInfo;
struct CreateSchemaInfo;
struct CreateTableFunctionInfo;
struct CreateCopyFunctionInfo;
struct CreateTypeInfo;

struct DropInfo;

//! A schema in the catalog
class SchemaCatalogEntry : public InCatalogEntry {
public:
	static constexpr const CatalogType Type = CatalogType::SCHEMA_ENTRY;
	static constexpr const char *Name = "schema";

public:
	SchemaCatalogEntry(Catalog &catalog, CreateSchemaInfo &info);

public:
	unique_ptr<CreateInfo> GetInfo() const override;

	//! Scan the specified catalog set, invoking the callback method for every entry
	virtual void Scan(ClientContext &context, CatalogType type,
	                  const std::function<void(CatalogEntry &)> &callback) = 0;
	//! Scan the specified catalog set, invoking the callback method for every committed entry
	virtual void Scan(CatalogType type, const std::function<void(CatalogEntry &)> &callback) = 0;

	string ToSQL() const override;

	//! Creates an index with the given name in the schema
	virtual optional_ptr<CatalogEntry> CreateIndex(CatalogTransaction transaction, CreateIndexInfo &info,
	                                               TableCatalogEntry &table) = 0;
	optional_ptr<CatalogEntry> CreateIndex(ClientContext &context, CreateIndexInfo &info, TableCatalogEntry &table);
	//! Create a scalar or aggregate function within the given schema
	virtual optional_ptr<CatalogEntry> CreateFunction(CatalogTransaction transaction, CreateFunctionInfo &info) = 0;
	//! Creates a table with the given name in the schema
	virtual optional_ptr<CatalogEntry> CreateTable(CatalogTransaction transaction, BoundCreateTableInfo &info) = 0;
	//! Creates a view with the given name in the schema
	virtual optional_ptr<CatalogEntry> CreateView(CatalogTransaction transaction, CreateViewInfo &info) = 0;
	//! Creates a sequence with the given name in the schema
	virtual optional_ptr<CatalogEntry> CreateSequence(CatalogTransaction transaction, CreateSequenceInfo &info) = 0;
	//! Create a table function within the given schema
	virtual optional_ptr<CatalogEntry> CreateTableFunction(CatalogTransaction transaction,
	                                                       CreateTableFunctionInfo &info) = 0;
	//! Create a copy function within the given schema
	virtual optional_ptr<CatalogEntry> CreateCopyFunction(CatalogTransaction transaction,
	                                                      CreateCopyFunctionInfo &info) = 0;
	//! Create a pragma function within the given schema
	virtual optional_ptr<CatalogEntry> CreatePragmaFunction(CatalogTransaction transaction,
	                                                        CreatePragmaFunctionInfo &info) = 0;
	//! Create a collation within the given schema
	virtual optional_ptr<CatalogEntry> CreateCollation(CatalogTransaction transaction, CreateCollationInfo &info) = 0;
	//! Create a enum within the given schema
	virtual optional_ptr<CatalogEntry> CreateType(CatalogTransaction transaction, CreateTypeInfo &info) = 0;

	//! Lookup an entry in the schema
	GOOSE_API virtual optional_ptr<CatalogEntry> LookupEntry(CatalogTransaction transaction,
	                                                          const EntryLookupInfo &lookup_info) = 0;
	GOOSE_API virtual CatalogSet::EntryLookup LookupEntryDetailed(CatalogTransaction transaction,
	                                                               const EntryLookupInfo &lookup_info);
	GOOSE_API virtual SimilarCatalogEntry GetSimilarEntry(CatalogTransaction transaction,
	                                                       const EntryLookupInfo &lookup_info);

	GOOSE_API optional_ptr<CatalogEntry> GetEntry(CatalogTransaction transaction, CatalogType type,
	                                               const string &name);

	//! Drops an entry from the schema
	virtual void DropEntry(ClientContext &context, DropInfo &info) = 0;

	//! Alters a catalog entry
	virtual void Alter(CatalogTransaction transaction, AlterInfo &info) = 0;

	CatalogTransaction GetCatalogTransaction(ClientContext &context);
};
} // namespace goose
