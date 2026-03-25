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
//


#include <goose/catalog/catalog_entry.h>

#include <goose/catalog/catalog.h>
#include <goose/common/serializer/binary_deserializer.h>
#include <goose/common/serializer/binary_serializer.h>
#include <goose/main/database.h>
#include <goose/main/database_manager.h>
#include <goose/parser/parsed_data/create_info.h>

namespace goose {

CatalogEntry::CatalogEntry(CatalogType type, string name_p, idx_t oid)
    : oid(oid), type(type), set(nullptr), name(std::move(name_p)), deleted(false), temporary(false), internal(false),
      parent(nullptr) {
}

CatalogEntry::CatalogEntry(CatalogType type, Catalog &catalog, string name_p)
    : CatalogEntry(type, std::move(name_p), catalog.GetDatabase().GetDatabaseManager().NextOid()) {
}

CatalogEntry::~CatalogEntry() {
}

void CatalogEntry::SetAsRoot() {
}

// LCOV_EXCL_START
unique_ptr<CatalogEntry> CatalogEntry::AlterEntry(ClientContext &context, AlterInfo &info) {
	throw InternalException("Unsupported alter type for catalog entry!");
}

unique_ptr<CatalogEntry> CatalogEntry::AlterEntry(CatalogTransaction transaction, AlterInfo &info) {
	if (!transaction.context) {
		throw InternalException("Cannot AlterEntry without client context");
	}
	return AlterEntry(*transaction.context, info);
}

void CatalogEntry::UndoAlter(ClientContext &context, AlterInfo &info) {
}

unique_ptr<CatalogEntry> CatalogEntry::Copy(ClientContext &context) const {
	throw InternalException("Unsupported copy type for catalog entry!");
}

unique_ptr<CreateInfo> CatalogEntry::GetInfo() const {
	throw InternalException("Unsupported type for CatalogEntry::GetInfo!");
}

string CatalogEntry::ToSQL() const {
	throw InternalException({{"catalog_type", CatalogTypeToString(type)}}, "Unsupported catalog type for ToSQL()");
}

void CatalogEntry::SetChild(unique_ptr<CatalogEntry> child_p) {
	child = std::move(child_p);
	if (child) {
		child->parent = this;
	}
}

unique_ptr<CatalogEntry> CatalogEntry::TakeChild() {
	if (child) {
		child->parent = nullptr;
	}
	return std::move(child);
}

bool CatalogEntry::HasChild() const {
	return child != nullptr;
}
bool CatalogEntry::HasParent() const {
	return parent != nullptr;
}

CatalogEntry &CatalogEntry::Child() {
	return *child;
}

CatalogEntry &CatalogEntry::Parent() {
	return *parent;
}

Catalog &CatalogEntry::ParentCatalog() {
	throw InternalException("CatalogEntry::ParentCatalog called on catalog entry without catalog");
}

const Catalog &CatalogEntry::ParentCatalog() const {
	throw InternalException("CatalogEntry::ParentCatalog called on catalog entry without catalog");
}

SchemaCatalogEntry &CatalogEntry::ParentSchema() {
	throw InternalException("CatalogEntry::ParentSchema called on catalog entry without schema");
}

const SchemaCatalogEntry &CatalogEntry::ParentSchema() const {
	throw InternalException("CatalogEntry::ParentSchema called on catalog entry without schema");
}
// LCOV_EXCL_STOP

void CatalogEntry::Serialize(Serializer &serializer) const {
	const auto info = GetInfo();
	info->Serialize(serializer);
}

unique_ptr<CreateInfo> CatalogEntry::Deserialize(Deserializer &deserializer) {
	return CreateInfo::Deserialize(deserializer);
}

void CatalogEntry::Verify(Catalog &catalog_p) {
}

void CatalogEntry::Rollback(CatalogEntry &prev_entry) {
}

void CatalogEntry::OnDrop() {
}

InCatalogEntry::InCatalogEntry(CatalogType type, Catalog &catalog, string name)
    : CatalogEntry(type, catalog, std::move(name)), catalog(catalog) {
}

InCatalogEntry::~InCatalogEntry() {
}

void InCatalogEntry::Verify(Catalog &catalog_p) {
	D_ASSERT(&catalog_p == &catalog);
}
} // namespace goose
