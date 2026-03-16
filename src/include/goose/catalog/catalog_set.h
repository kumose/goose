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
#include <goose/catalog/default/default_generator.h>
#include <goose/common/common.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/parser/column_definition.h>
#include <goose/transaction/transaction.h>
#include <goose/catalog/catalog_transaction.h>
#include <goose/catalog/similar_catalog_entry.h>
#include <functional>
#include <memory>

namespace goose {
struct AlterInfo;
struct ChangeOwnershipInfo;

class ClientContext;
class LogicalDependencyList;

class GooseCatalog;
class TableCatalogEntry;
class SequenceCatalogEntry;

class CatalogEntryMap {
public:
	CatalogEntryMap() {
	}

public:
	void AddEntry(unique_ptr<CatalogEntry> entry);
	void UpdateEntry(unique_ptr<CatalogEntry> entry);
	void DropEntry(CatalogEntry &entry);
	case_insensitive_tree_t<unique_ptr<CatalogEntry>> &Entries();
	optional_ptr<CatalogEntry> GetEntry(const string &name);

private:
	//! Mapping of string to catalog entry
	case_insensitive_tree_t<unique_ptr<CatalogEntry>> entries;
};

//! The Catalog Set stores (key, value) map of a set of CatalogEntries
class CatalogSet {
public:
	struct EntryLookup {
		enum class FailureReason { SUCCESS, DELETED, NOT_PRESENT, INVISIBLE };
		optional_ptr<CatalogEntry> result;
		FailureReason reason;
	};

public:
	GOOSE_API explicit CatalogSet(Catalog &catalog, unique_ptr<DefaultGenerator> defaults = nullptr);
	~CatalogSet();

	//! Create an entry in the catalog set. Returns whether or not it was
	//! successful.
	GOOSE_API bool CreateEntry(CatalogTransaction transaction, const string &name, unique_ptr<CatalogEntry> value,
	                            const LogicalDependencyList &dependencies);
	GOOSE_API bool CreateEntry(ClientContext &context, const string &name, unique_ptr<CatalogEntry> value,
	                            const LogicalDependencyList &dependencies);

	GOOSE_API bool AlterEntry(CatalogTransaction transaction, const string &name, AlterInfo &alter_info);

	GOOSE_API bool DropEntry(CatalogTransaction transaction, const string &name, bool cascade,
	                          bool allow_drop_internal = false);
	GOOSE_API bool DropEntry(ClientContext &context, const string &name, bool cascade,
	                          bool allow_drop_internal = false);
	//! Verify that the entry referenced by the dependency is still alive
	GOOSE_API void VerifyExistenceOfDependency(transaction_t commit_id, CatalogEntry &entry);
	//! Verify we can still drop the entry while committing
	GOOSE_API void CommitDrop(transaction_t commit_id, transaction_t start_time, CatalogEntry &entry);

	GOOSE_API GooseCatalog &GetCatalog();

	bool AlterOwnership(CatalogTransaction transaction, ChangeOwnershipInfo &info);

	void CleanupEntry(CatalogEntry &catalog_entry);

	//! Returns the entry with the specified name
	GOOSE_API EntryLookup GetEntryDetailed(CatalogTransaction transaction, const string &name);
	GOOSE_API optional_ptr<CatalogEntry> GetEntry(CatalogTransaction transaction, const string &name);
	GOOSE_API optional_ptr<CatalogEntry> GetEntry(ClientContext &context, const string &name);

	//! Gets the entry that is most similar to the given name (i.e. smallest levenshtein distance), or empty string if
	//! none is found. The returned pair consists of the entry name and the distance (smaller means closer).
	SimilarCatalogEntry SimilarEntry(CatalogTransaction transaction, const string &name);

	//! Rollback <entry> to be the currently valid entry for a certain catalog
	//! entry
	void Undo(CatalogEntry &entry);

	//! Scan the catalog set, invoking the callback method for every committed entry
	GOOSE_API void Scan(const std::function<void(CatalogEntry &)> &callback);
	//! Scan the catalog set, invoking the callback method for every entry
	GOOSE_API void ScanWithPrefix(CatalogTransaction transaction, const std::function<void(CatalogEntry &)> &callback,
	                               const string &prefix);
	GOOSE_API void Scan(CatalogTransaction transaction, const std::function<void(CatalogEntry &)> &callback);
	GOOSE_API void ScanWithReturn(CatalogTransaction transaction, const std::function<bool(CatalogEntry &)> &callback);
	GOOSE_API void Scan(ClientContext &context, const std::function<void(CatalogEntry &)> &callback);
	GOOSE_API void ScanWithReturn(ClientContext &context, const std::function<bool(CatalogEntry &)> &callback);

	template <class T>
	vector<reference<T>> GetEntries(CatalogTransaction transaction) {
		vector<reference<T>> result;
		Scan(transaction, [&](CatalogEntry &entry) { result.push_back(entry.Cast<T>()); });
		return result;
	}

	GOOSE_API bool CreatedByOtherActiveTransaction(CatalogTransaction transaction, transaction_t timestamp);
	GOOSE_API bool CommittedAfterStarting(CatalogTransaction transaction, transaction_t timestamp);
	GOOSE_API bool HasConflict(CatalogTransaction transaction, transaction_t timestamp);
	GOOSE_API bool UseTimestamp(CatalogTransaction transaction, transaction_t timestamp);
	static bool IsCommitted(transaction_t timestamp);

	static void UpdateTimestamp(CatalogEntry &entry, transaction_t timestamp);

	mutex &GetCatalogLock() {
		return catalog_lock;
	}

	void Verify(Catalog &catalog);

	//! Override the default generator - this should not be used after the catalog set has been used
	void SetDefaultGenerator(unique_ptr<DefaultGenerator> defaults);

private:
	bool DropDependencies(CatalogTransaction transaction, const string &name, bool cascade,
	                      bool allow_drop_internal = false);
	//! Given a root entry, gets the entry valid for this transaction, 'visible' is used to indicate whether the entry
	//! is actually visible to the transaction
	CatalogEntry &GetEntryForTransaction(CatalogTransaction transaction, CatalogEntry &current, bool &visible);
	//! Given a root entry, gets the entry valid for this transaction
	CatalogEntry &GetEntryForTransaction(CatalogTransaction transaction, CatalogEntry &current);
	CatalogEntry &GetCommittedEntry(CatalogEntry &current);
	optional_ptr<CatalogEntry> GetEntryInternal(CatalogTransaction transaction, const string &name);
	optional_ptr<CatalogEntry> CreateCommittedEntry(unique_ptr<CatalogEntry> entry);

	//! Create all default entries
	void CreateDefaultEntries(CatalogTransaction transaction, unique_lock<mutex> &lock);
	//! Attempt to create a default entry with the specified name. Returns the entry if successful, nullptr otherwise.
	optional_ptr<CatalogEntry> CreateDefaultEntry(CatalogTransaction transaction, const string &name,
	                                              unique_lock<mutex> &lock);

	bool DropEntryInternal(CatalogTransaction transaction, const string &name, bool allow_drop_internal = false);

	bool CreateEntryInternal(CatalogTransaction transaction, const string &name, unique_ptr<CatalogEntry> value,
	                         unique_lock<mutex> &read_lock, bool should_be_empty = true);
	void CheckCatalogEntryInvariants(CatalogEntry &value, const string &name);
	//! Verify that the previous entry in the chain is dropped.
	bool VerifyVacancy(CatalogTransaction transaction, CatalogEntry &entry);
	//! Start the catalog entry chain with a dummy node
	bool StartChain(CatalogTransaction transaction, const string &name, unique_lock<mutex> &read_lock);
	bool RenameEntryInternal(CatalogTransaction transaction, CatalogEntry &old, const string &new_name,
	                         AlterInfo &alter_info, unique_lock<mutex> &read_lock);

private:
	GooseCatalog &catalog;
	//! The catalog lock is used to make changes to the data
	mutex catalog_lock;
	CatalogEntryMap map;
	//! The generator used to generate default internal entries
	unique_ptr<DefaultGenerator> defaults;
};
} // namespace goose
