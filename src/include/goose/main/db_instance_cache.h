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

#include <goose/main/connection_manager.h>
#include <goose/main/database.h>
#include <goose/common/types-import.h>
#include <functional>

namespace goose {
class DBInstanceCache;
class DatabaseFilePathManager;

struct DatabaseCacheEntry {
	DatabaseCacheEntry();
	explicit DatabaseCacheEntry(const shared_ptr<Goose> &database);
	~DatabaseCacheEntry();

	weak_ptr<Goose> database;
	mutex update_database_mutex;
};

enum class CacheBehavior { AUTOMATIC, ALWAYS_CACHE, NEVER_CACHE };

class DBInstanceCache {
public:
	DBInstanceCache();
	~DBInstanceCache();

	//! Gets a DB Instance from the cache if already exists (Fails if the configurations do not match)
	shared_ptr<Goose> GetInstance(const string &database, const DBConfig &config_dict);

	//! Creates and caches a new DB Instance (Fails if a cached instance already exists)
	shared_ptr<Goose> CreateInstance(const string &database, DBConfig &config_dict, bool cache_instance = true,
	                                  const std::function<void(Goose &)> &on_create = nullptr);

	//! Either returns an existing entry, or creates and caches a new DB Instance
	shared_ptr<Goose> GetOrCreateInstance(const string &database, DBConfig &config_dict, bool cache_instance,
	                                       const std::function<void(Goose &)> &on_create = nullptr);
	shared_ptr<Goose> GetOrCreateInstance(const string &database, DBConfig &config_dict,
	                                       CacheBehavior cache_behavior = CacheBehavior::AUTOMATIC,
	                                       const std::function<void(Goose &)> &on_create = nullptr);

private:
	shared_ptr<DatabaseFilePathManager> path_manager;
	//! A map with the cached instances <absolute_path/instance>
	unordered_map<string, weak_ptr<DatabaseCacheEntry>> db_instances;

	//! Lock to alter cache
	mutex cache_lock;

private:
	shared_ptr<Goose> GetInstanceInternal(const string &database, const DBConfig &config,
	                                       std::unique_lock<std::mutex> &db_instances_lock);
	shared_ptr<Goose> CreateInstanceInternal(const string &database, DBConfig &config_dict, bool cache_instance,
	                                          std::unique_lock<std::mutex> db_instances_lock,
	                                          const std::function<void(Goose &)> &on_create);
};
} // namespace goose
