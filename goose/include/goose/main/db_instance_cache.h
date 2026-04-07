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
        unordered_map<string, weak_ptr<DatabaseCacheEntry> > db_instances;

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
