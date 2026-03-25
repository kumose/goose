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
#include <goose/main/extension_install_info.h>

namespace goose {
    class ErrorData;

    class ExtensionInfo {
    public:
        ExtensionInfo();

        mutex lock;
        atomic<bool> is_loaded;
        unique_ptr<ExtensionInstallInfo> install_info;
        unique_ptr<ExtensionLoadedInfo> load_info;
    };

    class ExtensionActiveLoad {
    public:
        ExtensionActiveLoad(DatabaseInstance &db, ExtensionInfo &info, string extension_name);

        DatabaseInstance &db;
        unique_lock<mutex> load_lock;
        ExtensionInfo &info;
        string extension_name;

    public:
        void FinishLoad(ExtensionInstallInfo &install_info);

        void LoadFail(const ErrorData &error);
    };

    class ExtensionManager {
    public:
        explicit ExtensionManager(DatabaseInstance &db);

        GOOSE_API bool ExtensionIsLoaded(const string &name);

        GOOSE_API vector<string> GetExtensions();

        GOOSE_API optional_ptr<ExtensionInfo> GetExtensionInfo(const string &name);

        GOOSE_API unique_ptr<ExtensionActiveLoad> BeginLoad(const string &extension);

        GOOSE_API static ExtensionManager &Get(DatabaseInstance &db);

        GOOSE_API static ExtensionManager &Get(ClientContext &context);

    private:
        DatabaseInstance &db;
        mutex lock;
        unordered_map<string, unique_ptr<ExtensionInfo> > loaded_extensions_info;
    };
} // namespace goose
