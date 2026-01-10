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
