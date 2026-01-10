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

#include <goose/catalog/catalog_entry_map.h>
#include <goose/common/types/hash.h>
#include <goose/common/enums/catalog_type.h>
#include <goose/catalog/dependency.h>

namespace goose {
class Catalog;
class CatalogEntry;
struct CreateInfo;
class SchemaCatalogEntry;
struct CatalogTransaction;
class LogicalDependencyList;

//! A minimal representation of a CreateInfo / CatalogEntry
//! enough to look up the entry inside SchemaCatalogEntry::GetEntry
struct LogicalDependency {
public:
	CatalogEntryInfo entry;
	string catalog;

public:
	explicit LogicalDependency(CatalogEntry &entry);
	LogicalDependency();
	LogicalDependency(optional_ptr<Catalog> catalog, CatalogEntryInfo entry, string catalog_str);
	bool operator==(const LogicalDependency &other) const;

public:
	void Serialize(Serializer &serializer) const;
	static LogicalDependency Deserialize(Deserializer &deserializer);
};

struct LogicalDependencyHashFunction {
	uint64_t operator()(const LogicalDependency &a) const;
};

struct LogicalDependencyEquality {
	bool operator()(const LogicalDependency &a, const LogicalDependency &b) const;
};

//! The LogicalDependencyList containing LogicalDependency objects, not looked up in the catalog yet
class LogicalDependencyList {
	using create_info_set_t =
	    unordered_set<LogicalDependency, LogicalDependencyHashFunction, LogicalDependencyEquality>;

public:
	GOOSE_API void AddDependency(CatalogEntry &entry);
	GOOSE_API void AddDependency(const LogicalDependency &entry);
	GOOSE_API bool Contains(CatalogEntry &entry);

public:
	GOOSE_API void VerifyDependencies(Catalog &catalog, const string &name);
	void Serialize(Serializer &serializer) const;
	static LogicalDependencyList Deserialize(Deserializer &deserializer);
	bool operator==(const LogicalDependencyList &other) const;
	const create_info_set_t &Set() const;

private:
	create_info_set_t set;
};

} // namespace goose
