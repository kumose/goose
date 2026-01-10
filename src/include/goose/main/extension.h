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
	    "4\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"};

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
