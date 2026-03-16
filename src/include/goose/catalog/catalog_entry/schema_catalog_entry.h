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
