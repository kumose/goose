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

#include <goose/common/winapi.h>
#include <goose/main/capi/extension_api.h>
#include <goose/main/config.h>
#include <goose/main/extension.h>
#include <goose/main/valid_checker.h>
#include <goose/main/extension/extension_loader.h>
#include <goose/main/extension_manager.h>

namespace goose {
    class BufferManager;
    class DatabaseManager;
    class StorageManager;
    class Catalog;
    class TransactionManager;
    class ConnectionManager;
    class ExtensionManager;
    class FileSystem;
    class TaskScheduler;
    class ObjectCache;
    struct AttachInfo;
    struct AttachOptions;
    class DatabaseFileSystem;
    struct DatabaseCacheEntry;
    class LogManager;
    class ExternalFileCache;
    class ResultSetManager;

    class DatabaseInstance : public enable_shared_from_this<DatabaseInstance> {
        friend class Goose;

    public:
        GOOSE_API DatabaseInstance();

        GOOSE_API ~DatabaseInstance();

        DBConfig config;

    public:
        BufferPool &GetBufferPool() const;

        GOOSE_API SecretManager &GetSecretManager();

        GOOSE_API BufferManager &GetBufferManager();

        GOOSE_API const BufferManager &GetBufferManager() const;

        GOOSE_API DatabaseManager &GetDatabaseManager();

        GOOSE_API FileSystem &GetFileSystem();

        GOOSE_API ExternalFileCache &GetExternalFileCache();

        GOOSE_API ResultSetManager &GetResultSetManager();

        GOOSE_API TaskScheduler &GetScheduler();

        GOOSE_API ObjectCache &GetObjectCache();

        GOOSE_API ConnectionManager &GetConnectionManager();

        GOOSE_API ExtensionManager &GetExtensionManager();

        GOOSE_API ValidChecker &GetValidChecker();

        GOOSE_API LogManager &GetLogManager() const;

        GOOSE_API const goose_ext_api_v1 GetExtensionAPIV1();

        idx_t NumberOfThreads();

        GOOSE_API static DatabaseInstance &GetDatabase(ClientContext &context);

        GOOSE_API static const DatabaseInstance &GetDatabase(const ClientContext &context);

        GOOSE_API bool ExtensionIsLoaded(const string &name);

        GOOSE_API SettingLookupResult TryGetCurrentSetting(const string &key, Value &result) const;

        GOOSE_API shared_ptr<EncryptionUtil> GetEncryptionUtil();

        shared_ptr<AttachedDatabase> CreateAttachedDatabase(ClientContext &context, AttachInfo &info,
                                                            AttachOptions &options);

    private:
        void Initialize(const char *path, DBConfig *config);

        void LoadExtensionSettings();

        void CreateMainDatabase();

        void Configure(DBConfig &config, const char *path);

    private:
        shared_ptr<BufferManager> buffer_manager;
        unique_ptr<DatabaseManager> db_manager;
        unique_ptr<TaskScheduler> scheduler;
        unique_ptr<ObjectCache> object_cache;
        unique_ptr<ConnectionManager> connection_manager;
        unique_ptr<ExtensionManager> extension_manager;
        ValidChecker db_validity;
        unique_ptr<DatabaseFileSystem> db_file_system;
        unique_ptr<LogManager> log_manager;
        unique_ptr<ExternalFileCache> external_file_cache;
        unique_ptr<ResultSetManager> result_set_manager;

        goose_ext_api_v1 (*create_api_v1)();
    };

    //! The database object. This object holds the catalog and all the
    //! database-specific meta information.
    class Goose {
    public:
        GOOSE_API explicit Goose(const char *path = nullptr, DBConfig *config = nullptr);

        GOOSE_API explicit Goose(const string &path, DBConfig *config = nullptr);

        GOOSE_API explicit Goose(DatabaseInstance &instance);

        GOOSE_API ~Goose();

        //! Reference to the actual database instance
        shared_ptr<DatabaseInstance> instance;

    public:
        // Load a statically loaded extension by its class
        template<class T>
        void LoadStaticExtension() {
            T extension;
            auto &manager = ExtensionManager::Get(*instance);
            auto load_info = manager.BeginLoad(extension.Name());
            if (!load_info) {
                // already loaded - return
                return;
            }

            // Instantiate a new loader
            ExtensionLoader loader(*load_info);

            // Call the Load method of the extension
            extension.Load(loader);

            // Finalize the loading process
            loader.FinalizeLoad();

            ExtensionInstallInfo install_info;
            install_info.mode = ExtensionInstallMode::STATICALLY_LINKED;
            install_info.version = extension.Version();
            load_info->FinishLoad(install_info);
        }

        GOOSE_API FileSystem &GetFileSystem();

        GOOSE_API idx_t NumberOfThreads();

        GOOSE_API static const char *SourceID();

        GOOSE_API static const char *LibraryVersion();

        GOOSE_API static const char *ReleaseCodename();

        GOOSE_API static idx_t StandardVectorSize();

        GOOSE_API static string Platform();

        GOOSE_API bool ExtensionIsLoaded(const string &name);
    };
} // namespace goose
