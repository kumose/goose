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

#include <goose/main/extension_loader.h>
#include <goose/main/extension_helper.h>
#include <turbo/utility/status.h>
#include <mutex>
#include <turbo/container/flat_hash_map.h>
#include  <goose/version.h>

#if defined(STATIC_LOAD_CORE_FUNCTION)
#include <core_functions_extension.h>
#endif

#if defined(STATIC_LOAD_JSON)
#include <json_extension.h>
#endif

#if defined(STATIC_LOAD_PARQUET)
#include <parquet_extension.h>
#endif

#if defined(STATIC_LOAD_ICU)
#include <icu_extension.h>
#endif


namespace goose {
    static std::mutex extension_loader_mutex;
    static turbo::flat_hash_map<std::string, loader_function> extension_loaders = {
#if defined(STATIC_LOAD_CORE_FUNCTION)
        {
            "core_function", [](Goose &db) {
                db.LoadStaticExtension<CoreFunctionsExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            }
        },
#endif
#if defined(STATIC_LOAD_JSON)
        {
            "json", [](Goose &db) {
                db.LoadStaticExtension<JsonExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            }
        },
#endif
#if defined(STATIC_LOAD_PARQUET)
        {
            "parquet", [](Goose &db) {
                db.LoadStaticExtension<ParquetExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            }
        },
#endif
#if defined(STATIC_LOAD_ICU)
        {
            "icu", [](Goose &db) {
                db.LoadStaticExtension<IcuExtension>();
                return ExtensionLoadResult::LOADED_EXTENSION;
            }
        },
#endif
    };
    static std::mutex test_extension_loader_mutex;
    static turbo::flat_hash_map<std::string, loader_function> test_extension_loaders;

    turbo::Status enable_extension_autoload(std::string_view ext, loader_function loader) {
        if (ext.empty()) {
            return turbo::invalid_argument_error("extension is empty");
        }
        if (loader == nullptr) {
            return turbo::invalid_argument_error("loader is null");
        }
        std::lock_guard<std::mutex> lock(extension_loader_mutex);
        auto it = extension_loaders.find(ext);
        if (it != extension_loaders.end()) {
            return turbo::already_exists_error("already exists extension:", ext);
        }
        extension_loaders[ext] = loader;
        return turbo::OkStatus();
    }

    turbo::Status disable_extension_autoload(std::string_view ext) {
        if (ext.empty()) {
            return turbo::invalid_argument_error("extension is empty");
        }
        std::lock_guard<std::mutex> lock(extension_loader_mutex);
        auto it = extension_loaders.find(ext);
        if (it == extension_loaders.end()) {
            return turbo::not_found_error("not found extension:", ext);
        }
        extension_loaders.erase(ext);
        return turbo::OkStatus();
    }

    std::vector<std::string> auto_load_extensions() {
        std::vector<std::string> extensions;
        std::lock_guard<std::mutex> lock(extension_loader_mutex);
        for (auto it = extension_loaders.begin();it != extension_loaders.end(); ++it) {
            extensions.push_back(it->first);
        }
        return extensions;
    }


    turbo::Status enable_test_extension_autoload(std::string_view ext, loader_function loader) {
        if (ext.empty()) {
            return turbo::invalid_argument_error("extension is empty");
        }
        if (loader == nullptr) {
            return turbo::invalid_argument_error("loader is null");
        }
        std::lock_guard<std::mutex> lock(test_extension_loader_mutex);
        auto it = test_extension_loaders.find(ext);
        if (it != test_extension_loaders.end()) {
            return turbo::already_exists_error("already exists extension:", ext);
        }
        test_extension_loaders[ext] = loader;
        return turbo::OkStatus();
    }

    turbo::Status disable_test_extension_autoload(std::string_view ext) {
        if (ext.empty()) {
            return turbo::invalid_argument_error("extension is empty");
        }
        std::lock_guard<std::mutex> lock(test_extension_loader_mutex);
        auto it = test_extension_loaders.find(ext);
        if (it == test_extension_loaders.end()) {
            return turbo::not_found_error("not found extension:", ext);
        }
        test_extension_loaders.erase(ext);
        return turbo::OkStatus();
    }

    std::vector<std::string> auto_load_test_extensions() {
        std::vector<std::string> extensions;
        std::lock_guard<std::mutex> lock(test_extension_loader_mutex);
        for (auto it = test_extension_loaders.begin();it != test_extension_loaders.end(); ++it) {
            extensions.push_back(it->first);
        }
        return extensions;
    }

    void ExtensionHelper::LoadAllExtensions(Goose &db) {
        std::unique_lock<std::mutex> lock(extension_loader_mutex);
        for (auto it: extension_loaders) {
            it.second(db);
        }
    }

    vector<string> ExtensionHelper::LoadedExtensionTestPaths() {
        return auto_load_test_extensions();
    }

    ExtensionLoadResult ExtensionHelper::LoadExtension(Goose &db, const std::string &extension) {
        std::unique_lock<std::mutex> lock(extension_loader_mutex);
        auto it = extension_loaders.find(extension);
        if (it == extension_loaders.end()) {
            return ExtensionLoadResult::NOT_LOADED;
        }
        return it->second(db);
    }
} // namespace goose
