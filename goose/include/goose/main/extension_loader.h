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

#pragma once

#include <string>
#include <functional>
#include <turbo/utility/status.h>
#include <turbo/log/logging.h>

namespace goose {
    class Goose;

    enum class ExtensionLoadResult : uint8_t { LOADED_EXTENSION = 0, EXTENSION_UNKNOWN = 1, NOT_LOADED = 2 };

    using loader_function = std::function<ExtensionLoadResult(Goose &)>;


    turbo::Status enable_extension_autoload(std::string_view ext, loader_function loader);

    turbo::Status disable_extension_autoload(std::string_view ext);

    std::vector<std::string> auto_load_extensions();

    turbo::Status enable_test_extension_autoload(std::string_view ext, loader_function loader);

    turbo::Status disable_test_extension_autoload(std::string_view ext);

    std::vector<std::string> auto_load_test_extensions();

    template<typename T>
    struct AutoLoaderRegister {
        AutoLoaderRegister(std::string name, loader_function loader) {
            auto rs = enable_extension_autoload(name, loader);
            KCHECK(rs.ok()) << "registering autoload extension: " << name << " failed, reason: " << rs.to_string();
        }

        ~AutoLoaderRegister() = default;
    };

    template<typename T>
    struct AutoLoaderTestRegister {
        AutoLoaderTestRegister(std::string name, loader_function loader) {
            auto rs = enable_test_extension_autoload(name, loader);
            KCHECK(rs.ok()) << "registering autoload test extension: " << name << " failed, reason: " << rs.to_string();
        }

        ~AutoLoaderTestRegister() = default;
    };
} // namespace goose

#define AUTO_REGISTRY_EXTENSION(type, name, loader) \
    ::goose::AutoLoaderRegister<type> __extension_var_##name(name loader)

#define AUTO_REGISTRY_TEST_EXTENSION(type, name, loader) \
    ::goose::AutoLoaderTestRegister<type> __test_extension_var_##name(name loader)
