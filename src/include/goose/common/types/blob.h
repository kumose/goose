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
#include <goose/common/types.h>

namespace goose {
struct CastParameters;

//! The Blob class is a static class that holds helper functions for the Blob type.
class Blob {
public:
	// map of integer -> hex value
	static constexpr const char *HEX_TABLE = "0123456789ABCDEF";
	// reverse map of byte -> integer value, or -1 for invalid hex values
	static const int HEX_MAP[256];
	//! map of index -> base64 character
	static constexpr const char *BASE64_MAP = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//! padding character used in base64 encoding
	static constexpr const char BASE64_PADDING = '=';

public:
	//! Returns the string size of a blob -> string conversion
	GOOSE_API static idx_t GetStringSize(string_t blob);
	//! Converts a blob to a string, writing the output to the designated output string.
	//! The string needs to have space for at least GetStringSize(blob) bytes.
	GOOSE_API static void ToString(string_t blob, char *output);
	//! Convert a blob object to a string
	GOOSE_API static string ToString(string_t blob);

	//! Returns the blob size of a string -> blob conversion
	GOOSE_API static bool TryGetBlobSize(string_t str, idx_t &result_size, CastParameters &parameters);
	GOOSE_API static idx_t GetBlobSize(string_t str);
	GOOSE_API static idx_t GetBlobSize(string_t str, CastParameters &parameters);
	//! Convert a string to a blob. This function should ONLY be called after calling GetBlobSize, since it does NOT
	//! perform data validation.
	GOOSE_API static void ToBlob(string_t str, data_ptr_t output);
	//! Convert a string object to a blob
	GOOSE_API static string ToBlob(string_t str);
	GOOSE_API static string ToBlob(string_t str, CastParameters &parameters);

	// base 64 conversion functions
	GOOSE_API static string ToBase64(string_t blob);
	//! Returns the string size of a blob -> base64 conversion
	GOOSE_API static idx_t ToBase64Size(string_t blob);
	//! Converts a blob to a base64 string, output should have space for at least ToBase64Size(blob) bytes
	GOOSE_API static void ToBase64(string_t blob, char *output);

	GOOSE_API static string FromBase64(string_t blob);
	//! Returns the string size of a base64 string -> blob conversion
	GOOSE_API static idx_t FromBase64Size(string_t str);
	//! Converts a base64 string to a blob, output should have space for at least FromBase64Size(blob) bytes
	GOOSE_API static void FromBase64(string_t str, data_ptr_t output, idx_t output_size);
};
} // namespace goose
