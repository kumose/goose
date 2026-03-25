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
#include <goose/common/types-import.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/common/enums/on_create_conflict.h>
#include <goose/common/enums/access_mode.h>
#include <goose/common/reference_map.h>

namespace goose {
struct AttachInfo;
struct AttachOptions;
class DatabaseManager;

enum class InsertDatabasePathResult { SUCCESS, ALREADY_EXISTS };

struct DatabasePathInfo {
	DatabasePathInfo(DatabaseManager &manager, string name_p, AccessMode access_mode);

	string name;
	AccessMode access_mode;
	reference_set_t<DatabaseManager> attached_databases;
	idx_t reference_count = 1;
};

//! The DatabaseFilePathManager is used to ensure we only ever open a single database file once
class DatabaseFilePathManager {
public:
	idx_t ApproxDatabaseCount() const;
	InsertDatabasePathResult InsertDatabasePath(DatabaseManager &manager, const string &path, const string &name,
	                                            OnCreateConflict on_conflict, AttachOptions &options);
	//! Erase a database path - indicating we are done with using it
	void EraseDatabasePath(const string &path);
	//! Called when a database is detached, but before it is fully finished being used
	void DetachDatabase(DatabaseManager &manager, const string &path);

private:
	//! The lock to add entries to the db_paths map
	mutable mutex db_paths_lock;
	//! A set containing all attached database path
	//! This allows to attach many databases efficiently, and to avoid attaching the
	//! same file path twice
#if defined(_WIN32) || defined(__APPLE__)
	case_insensitive_map_t<DatabasePathInfo> db_paths;
#else  // !(_WIN32 or __APPLE__)
	unordered_map<string, DatabasePathInfo> db_paths;
#endif // _WIN32 or __APPLE__
};

} // namespace goose
