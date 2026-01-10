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
#include <goose/catalog/dependency.h>
#include <goose/catalog/catalog_entry_map.h>
#include <goose/catalog/catalog_transaction.h>
#include <goose/common/types-import.h>
#include <goose/common/string_util.h>

#include <functional>

namespace goose {
class GooseCatalog;
class ClientContext;
class DependencyEntry;
class LogicalDependencyList;

// The subject of this dependency
struct DependencySubject {
	CatalogEntryInfo entry;
	//! The type of dependency this is (e.g, ownership)
	DependencySubjectFlags flags;
};

// The entry that relies on the other entry
struct DependencyDependent {
	CatalogEntryInfo entry;
	//! The type of dependency this is (e.g, blocking, non-blocking, ownership)
	DependencyDependentFlags flags;
};

//! Every dependency consists of a subject (the entry being depended on) and a dependent (the entry that has the
//! dependency)
struct DependencyInfo {
public:
	static DependencyInfo FromSubject(DependencyEntry &dep);
	static DependencyInfo FromDependent(DependencyEntry &dep);

public:
	DependencyDependent dependent;
	DependencySubject subject;
};

struct MangledEntryName {
public:
	explicit MangledEntryName(const CatalogEntryInfo &info);
	MangledEntryName() = delete;

public:
	//! Format: Type\0Schema\0Name
	string name;

public:
	bool operator==(const MangledEntryName &other) const {
		return StringUtil::CIEquals(other.name, name);
	}
	bool operator!=(const MangledEntryName &other) const {
		return !(*this == other);
	}
};

struct MangledDependencyName {
public:
	MangledDependencyName(const MangledEntryName &from, const MangledEntryName &to);
	MangledDependencyName() = delete;

public:
	//! Format: MangledEntryName\0MangledEntryName
	string name;
};

//! The DependencyManager is in charge of managing dependencies between catalog entries
class DependencyManager {
	friend class CatalogSet;

public:
	explicit DependencyManager(GooseCatalog &catalog);

	//! Scans all dependencies, returning pairs of (object, dependent)
	void Scan(ClientContext &context,
	          const std::function<void(CatalogEntry &, CatalogEntry &, const DependencyDependentFlags &)> &callback);

	void AddOwnership(CatalogTransaction transaction, CatalogEntry &owner, CatalogEntry &entry);

	//! Get the order of entries needed by EXPORT, the objects with no dependencies are exported first
	void ReorderEntries(catalog_entry_vector_t &entries);
	void ReorderEntries(catalog_entry_vector_t &entries, ClientContext &context);

private:
	GooseCatalog &catalog;
	CatalogSet subjects;
	CatalogSet dependents;

private:
	bool IsSystemEntry(CatalogEntry &entry) const;
	optional_ptr<CatalogEntry> LookupEntry(CatalogTransaction transaction, const LogicalDependency &dependency);
	optional_ptr<CatalogEntry> LookupEntry(CatalogTransaction transaction, CatalogEntry &dependency);
	string CollectDependents(CatalogTransaction transaction, catalog_entry_set_t &entries, CatalogEntryInfo &info);
	void CleanupDependencies(CatalogTransaction transaction, CatalogEntry &entry);

public:
	static string GetSchema(const CatalogEntry &entry);
	static MangledEntryName MangleName(const CatalogEntryInfo &info);
	static MangledEntryName MangleName(const CatalogEntry &entry);
	static CatalogEntryInfo GetLookupProperties(const CatalogEntry &entry);

private:
	void ReorderEntry(CatalogTransaction transaction, CatalogEntry &entry, catalog_entry_set_t &visited,
	                  catalog_entry_vector_t &order);
	void ReorderEntries(catalog_entry_vector_t &entries, CatalogTransaction transaction);
	void AddObject(CatalogTransaction transaction, CatalogEntry &object, const LogicalDependencyList &dependencies);
	void VerifyExistence(CatalogTransaction transaction, DependencyEntry &object);
	void VerifyCommitDrop(CatalogTransaction transaction, transaction_t start_time, CatalogEntry &object);
	//! Returns the objects that should be dropped alongside the object
	catalog_entry_set_t CheckDropDependencies(CatalogTransaction transaction, CatalogEntry &object, bool cascade);
	void DropObject(CatalogTransaction transaction, CatalogEntry &object, bool cascade);
	void AlterObject(CatalogTransaction transaction, CatalogEntry &old_obj, CatalogEntry &new_obj, AlterInfo &info);

private:
	void RemoveDependency(CatalogTransaction transaction, const DependencyInfo &info);
	void CreateDependency(CatalogTransaction transaction, DependencyInfo &info);
	void CreateDependencies(CatalogTransaction transaction, const CatalogEntry &object,
	                        const LogicalDependencyList &dependencies);
	using dependency_entry_func_t = const std::function<unique_ptr<DependencyEntry>(
	    Catalog &catalog, const DependencyDependent &dependent, const DependencySubject &dependency)>;

	void CreateSubject(CatalogTransaction transaction, const DependencyInfo &info);
	void CreateDependent(CatalogTransaction transaction, const DependencyInfo &info);

	using dependency_callback_t = const std::function<void(DependencyEntry &)>;
	void ScanDependents(CatalogTransaction transaction, const CatalogEntryInfo &info, dependency_callback_t &callback);
	void ScanSubjects(CatalogTransaction transaction, const CatalogEntryInfo &info, dependency_callback_t &callback);
	void ScanSetInternal(CatalogTransaction transaction, const CatalogEntryInfo &info, bool subjects,
	                     dependency_callback_t &callback);
	void PrintSubjects(CatalogTransaction transaction, const CatalogEntryInfo &info);
	void PrintDependents(CatalogTransaction transaction, const CatalogEntryInfo &info);
	CatalogSet &Dependents();
	CatalogSet &Subjects();
};

} // namespace goose
