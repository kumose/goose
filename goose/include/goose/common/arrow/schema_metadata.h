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
