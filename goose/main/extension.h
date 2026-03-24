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
#include <goose/common/winapi.h>

namespace goose {
    class ExtensionLoader;

    //! The Extension class is the base class used to define extensions
    class Extension {
    public:
        GOOSE_API virtual ~Extension();

        GOOSE_API virtual void Load(ExtensionLoader &db) = 0;

        GOOSE_API virtual std::string Name() = 0;

        GOOSE_API virtual std::string Version() const {
            return "";
        }

        GOOSE_API static const char *DefaultVersion();
    };

    enum class ExtensionABIType : uint8_t {
        UNKNOWN = 0,
        //! Uses C++ ABI, version needs to match precisely
        CPP = 1,
        //! Uses C ABI using the goose_ext_api_v1 struct, version needs to be equal or higher
        C_STRUCT = 2,
        //! Uses C ABI using the goose_ext_api_v1 struct including "unstable" functions, version needs to match precisely
        C_STRUCT_UNSTABLE = 3
    };

    //! The parsed extension metadata footer
    struct ParsedExtensionMetaData {
        static constexpr const idx_t FOOTER_SIZE = 512;
        static constexpr const idx_t SIGNATURE_SIZE = 256;
        static constexpr const char *EXPECTED_MAGIC_VALUE = {
            "4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        };

        string magic_value;

        ExtensionABIType abi_type;

        string platform;
        // (For ExtensionABIType::CPP or ExtensionABIType::C_STRUCT_UNSTABLE) the Goose version this extension is compiled
        // for
        string goose_version;
        // (only for ExtensionABIType::C_STRUCT) the CAPI version of the C_STRUCT (Currently interpreted as the minimum
        // Goose version)
        string goose_capi_version;
        string extension_version;
        string signature;
        string extension_abi_metadata;

        bool AppearsValid() {
            return magic_value == EXPECTED_MAGIC_VALUE;
        }

        // Returns an error string describing which parts of the metadata are mismatcheds
        string GetInvalidMetadataError();
    };

    struct VersioningUtils {
        //! Note: only supports format v{major}.{minor}.{patch}
        static bool ParseSemver(string &semver, idx_t &major_out, idx_t &minor_out, idx_t &patch_out);

        //! Note: only supports format v{major}.{minor}.{patch}
        static bool IsSupportedCAPIVersion(string &capi_version_string);

        static bool IsSupportedCAPIVersion(idx_t major, idx_t minor, idx_t patch);
    };
} // namespace goose
