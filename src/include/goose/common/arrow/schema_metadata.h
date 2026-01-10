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

#include <goose/main/query_result.h>
#include <goose/common/arrow/arrow_wrapper.h>
#include <goose/main/chunk_scan_state.h>
#include <goose/common/arrow/arrow_type_extension.h>
#include <goose/common/complex_json.h>

namespace goose {
class ArrowSchemaMetadata {
public:
	//! Constructor used to read a metadata schema, used when importing an arrow object
	explicit ArrowSchemaMetadata(const char *metadata);
	//! Constructor used to create a metadata schema, used when exporting an arrow object
	ArrowSchemaMetadata();
	//! Adds an option to the metadata
	void AddOption(const string &key, const string &value);
	//! Gets an option from the metadata, returns an empty string if it does not exist.
	string GetOption(const string &key) const;
	//! Transforms metadata to a char*, used when creating an arrow object
	unsafe_unique_array<char> SerializeMetadata() const;
	//! If the arrow extension is set
	bool HasExtension() const;

	ArrowExtensionMetadata GetExtensionInfo(string format);
	//! Get the extension name if set, otherwise returns empty
	string GetExtensionName() const;
	//! Key for encode of the extension type name
	static constexpr const char *ARROW_EXTENSION_NAME = "ARROW:extension:name";
	//! Key for encode of the metadata key
	static constexpr const char *ARROW_METADATA_KEY = "ARROW:extension:metadata";
	//! Creates the metadata based on an extension name
	static ArrowSchemaMetadata ArrowCanonicalType(const string &extension_name);
	//! Creates the metadata based on an extension name
	static ArrowSchemaMetadata NonCanonicalType(const string &type_name, const string &vendor_name);

private:
	//! The unordered map that holds the metadata
	unordered_map<string, string> schema_metadata_map;
	//! The extension metadata map, currently only used for internal types in arrow.opaque
	unique_ptr<ComplexJSON> extension_metadata_map;
};
} // namespace goose
