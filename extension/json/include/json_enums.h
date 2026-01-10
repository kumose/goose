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
