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

#include <goose/catalog/catalog_entry/schema_catalog_entry.h>

namespace goose {

//! A schema in the catalog
class GooseSchemaEntry : public SchemaCatalogEntry {
public:
	GooseSchemaEntry(Catalog &catalog, CreateSchemaInfo &info);

private:
	//! The catalog set holding the tables
	CatalogSet tables;
	//! The catalog set holding the indexes
	CatalogSet indexes;
	//! The catalog set holding the table functions
	CatalogSet table_functions;
	//! The catalog set holding the copy functions
	CatalogSet copy_functions;
	//! The catalog set holding the pragma functions
	CatalogSet pragma_functions;
	//! The catalog set holding the scalar and aggregate functions
	CatalogSet functions;
	//! The catalog set holding the sequences
	CatalogSet sequences;
	//! The catalog set holding the collations
	CatalogSet collations;
	//! The catalog set holding the types
	CatalogSet types;

public:
	optional_ptr<CatalogEntry> AddEntry(CatalogTransaction transaction, unique_ptr<StandardEntry> entry,
	                                    OnCreateConflict on_conflict);
	optional_ptr<CatalogEntry> AddEntryInternal(CatalogTransaction transaction, unique_ptr<StandardEntry> entry,
	                                            OnCreateConflict on_conflict, LogicalDependencyList dependencies);

	optional_ptr<CatalogEntry> CreateTable(CatalogTransaction transaction, BoundCreateTableInfo &info) override;
	optional_ptr<CatalogEntry> CreateFunction(CatalogTransaction transaction, CreateFunctionInfo &info) override;
	optional_ptr<CatalogEntry> CreateIndex(CatalogTransaction transaction, CreateIndexInfo &info,
	                                       TableCatalogEntry &table) override;
	optional_ptr<CatalogEntry> CreateView(CatalogTransaction transaction, CreateViewInfo &info) override;
	optional_ptr<CatalogEntry> CreateSequence(CatalogTransaction transaction, CreateSequenceInfo &info) override;
	optional_ptr<CatalogEntry> CreateTableFunction(CatalogTransaction transaction,
	                                               CreateTableFunctionInfo &info) override;
	optional_ptr<CatalogEntry> CreateCopyFunction(CatalogTransaction transaction,
	                                              CreateCopyFunctionInfo &info) override;
	optional_ptr<CatalogEntry> CreatePragmaFunction(CatalogTransaction transaction,
	                                                CreatePragmaFunctionInfo &info) override;
	optional_ptr<CatalogEntry> CreateCollation(CatalogTransaction transaction, CreateCollationInfo &info) override;
	optional_ptr<CatalogEntry> CreateType(CatalogTransaction transaction, CreateTypeInfo &info) override;
	void Alter(CatalogTransaction transaction, AlterInfo &info) override;
	void Scan(ClientContext &context, CatalogType type, const std::function<void(CatalogEntry &)> &callback) override;
	void Scan(CatalogType type, const std::function<void(CatalogEntry &)> &callback) override;
	void DropEntry(ClientContext &context, DropInfo &info) override;
	optional_ptr<CatalogEntry> LookupEntry(CatalogTransaction transaction, const EntryLookupInfo &lookup_info) override;
	CatalogSet::EntryLookup LookupEntryDetailed(CatalogTransaction transaction,
	                                            const EntryLookupInfo &lookup_info) override;
	SimilarCatalogEntry GetSimilarEntry(CatalogTransaction transaction, const EntryLookupInfo &lookup_info) override;

	unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;

	void Verify(Catalog &catalog) override;

	//! Get the catalog set for the specified type
	CatalogSet &GetCatalogSet(CatalogType type);

private:
	void OnDropEntry(CatalogTransaction transaction, CatalogEntry &entry);
};
} // namespace goose
