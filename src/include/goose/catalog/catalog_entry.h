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

#include <goose/common/common.h>
#include <goose/common/enums/catalog_type.h>
#include <goose/common/exception.h>
#include <goose/common/types-import.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/exception/catalog_exception.h>
#include <goose/common/types/value.h>
#include <memory>

namespace goose {

class Catalog;
class CatalogSet;
class ClientContext;
class Deserializer;
class SchemaCatalogEntry;
class Serializer;
class Value;

struct AlterInfo;
struct CatalogTransaction;
struct CreateInfo;

//! Abstract base class of an entry in the catalog
class CatalogEntry {
public:
	CatalogEntry(CatalogType type, Catalog &catalog, string name);
	CatalogEntry(CatalogType type, string name, idx_t oid);
	virtual ~CatalogEntry();

	//! The oid of the entry
	idx_t oid;
	//! The type of this catalog entry
	CatalogType type;
	//! Reference to the catalog set this entry is stored in
	optional_ptr<CatalogSet> set;
	//! The name of the entry
	string name;
	//! Whether or not the object is deleted
	bool deleted;
	//! Whether or not the object is temporary and should not be added to the WAL
	bool temporary;
	//! Whether or not the entry is an internal entry (cannot be deleted, not dumped, etc)
	bool internal;
	//! Timestamp at which the catalog entry was created
	atomic<transaction_t> timestamp;
	//! (optional) comment on this entry
	Value comment;
	//! (optional) extra data associated with this entry
	InsertionOrderPreservingMap<string> tags;

private:
	//! Child entry
	unique_ptr<CatalogEntry> child;
	//! Parent entry (the node that dependents_map this node)
	optional_ptr<CatalogEntry> parent;

public:
	virtual unique_ptr<CatalogEntry> AlterEntry(ClientContext &context, AlterInfo &info);
	virtual unique_ptr<CatalogEntry> AlterEntry(CatalogTransaction transaction, AlterInfo &info);
	virtual void UndoAlter(ClientContext &context, AlterInfo &info);
	virtual void Rollback(CatalogEntry &prev_entry);
	virtual void OnDrop();

	virtual unique_ptr<CatalogEntry> Copy(ClientContext &context) const;

	virtual unique_ptr<CreateInfo> GetInfo() const;

	//! Sets the CatalogEntry as the new root entry (i.e. the newest entry)
	// this is called on a rollback to an AlterEntry
	virtual void SetAsRoot();

	//! Convert the catalog entry to a SQL string that can be used to re-construct the catalog entry
	virtual string ToSQL() const;

	virtual Catalog &ParentCatalog();
	virtual const Catalog &ParentCatalog() const;
	virtual SchemaCatalogEntry &ParentSchema();
	virtual const SchemaCatalogEntry &ParentSchema() const;

	virtual void Verify(Catalog &catalog);

	void Serialize(Serializer &serializer) const;
	static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

public:
	void SetChild(unique_ptr<CatalogEntry> child);
	unique_ptr<CatalogEntry> TakeChild();
	bool HasChild() const;
	bool HasParent() const;
	CatalogEntry &Child();
	CatalogEntry &Parent();
	const CatalogEntry &Parent() const;

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

class InCatalogEntry : public CatalogEntry {
public:
	InCatalogEntry(CatalogType type, Catalog &catalog, string name);
	~InCatalogEntry() override;

	//! The catalog the entry belongs to
	Catalog &catalog;

public:
	Catalog &ParentCatalog() override {
		return catalog;
	}
	const Catalog &ParentCatalog() const override {
		return catalog;
	}

	void Verify(Catalog &catalog) override;
};

} // namespace goose
