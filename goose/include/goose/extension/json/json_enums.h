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

#include <goose/common/constants.h>
#include <goose/common/enum_util.h>

namespace goose {

enum class JSONScanType : uint8_t {
	INVALID = 0,
	//! Read JSON straight to columnar data
	READ_JSON = 1,
	//! Read JSON values as strings
	READ_JSON_OBJECTS = 2,
	//! Sample run for schema detection
	SAMPLE = 3,
};

enum class JSONRecordType : uint8_t {
	AUTO_DETECT = 0,
	//! Sequential objects that are unpacked
	RECORDS = 1,
	//! Any other JSON type, e.g., ARRAY
	VALUES = 2,
};

enum class JSONFormat : uint8_t {
	//! Auto-detect format (UNSTRUCTURED / NEWLINE_DELIMITED)
	AUTO_DETECT = 0,
	//! One unit after another, newlines can be anywhere
	UNSTRUCTURED = 1,
	//! Units are separated by newlines, newlines do not occur within Units (NDJSON)
	NEWLINE_DELIMITED = 2,
	//! File is one big array of units
	ARRAY = 3,
};

template<>
const char* EnumUtil::ToChars<JSONScanType>(JSONScanType value);

template<>
JSONScanType EnumUtil::FromString<JSONScanType>(const char *value);

template<>
const char* EnumUtil::ToChars<JSONRecordType>(JSONRecordType value);

template<>
JSONRecordType EnumUtil::FromString<JSONRecordType>(const char *value);

template<>
const char* EnumUtil::ToChars<JSONFormat>(JSONFormat value);

template<>
JSONFormat EnumUtil::FromString<JSONFormat>(const char *value);

} // namespace goose
