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
#include <goose/common/enums/compression_type.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/function/function.h>
#include <goose/storage/data_pointer.h>
#include <goose/storage/storage_info.h>

namespace goose {

struct DBConfig;

class EncodingFunction;

struct CSVEncoderBuffer;
//! Decode function, basically takes information about the decoded and the encoded buffers.
typedef void (*encode_t)(CSVEncoderBuffer &encoded_buffer, char *decoded_buffer, idx_t &decoded_buffer_current_position,
                         const idx_t decoded_buffer_size, char *remaining_bytes_buffer, idx_t &remaining_bytes_size,
                         EncodingFunction *encoding_function);

//! Encoding Map Entry Struct used for byte replacement
typedef struct {
	size_t key_len;
	const char *key;
	size_t value_len;
	const char *value;
} map_entry_encoding;

class EncodingFunction {
public:
	GOOSE_API EncodingFunction() : encode_function(nullptr), max_bytes_per_iteration(0) {
	}

	GOOSE_API EncodingFunction(const string &encode_name, encode_t encode_function, const idx_t bytes_per_iteration,
	                            const idx_t lookup_bytes)
	    : name(encode_name), encode_function(encode_function), max_bytes_per_iteration(bytes_per_iteration),
	      lookup_bytes(lookup_bytes) {
		D_ASSERT(encode_function);
		D_ASSERT(bytes_per_iteration > 0);
		D_ASSERT(lookup_bytes > 0);
	};

	GOOSE_API EncodingFunction(const string &encode_name, encode_t encode_function, const idx_t bytes_per_iteration,
	                            const idx_t lookup_bytes, const map_entry_encoding *map, const size_t map_size)
	    : conversion_map(map), map_size(map_size), name(encode_name), encode_function(encode_function),
	      max_bytes_per_iteration(bytes_per_iteration), lookup_bytes(lookup_bytes) {
		D_ASSERT(encode_function);
		D_ASSERT(bytes_per_iteration > 0);
		D_ASSERT(lookup_bytes > 0);
	};

	GOOSE_API ~EncodingFunction() {};

	GOOSE_API string GetName() const {
		return name;
	}
	GOOSE_API encode_t GetFunction() const {
		return encode_function;
	}
	GOOSE_API idx_t GetBytesPerIteration() const {
		return max_bytes_per_iteration;
	}
	GOOSE_API idx_t GetLookupBytes() const {
		return lookup_bytes;
	}

	//! Optional convertion map, that indicates byte replacements.
	const map_entry_encoding *conversion_map {};
	size_t map_size {};

protected:
	//! The encoding type of this function (e.g., utf-8)
	string name;
	//! The actual encoding function
	encode_t encode_function;
	//! How many bytes in the decoded buffer one iteration of the encoded function can cause.
	//! e.g., one iteration of Latin-1 to UTF-8 can generate max 2 bytes.
	//! However, one iteration of UTF-16 to UTF-8, can generate up to 3 UTF-8 bytes.
	idx_t max_bytes_per_iteration;
	//! How many bytes we have to lookup before knowing the bytes we have to output
	idx_t lookup_bytes = 1;
};

//! The set of encoding functions
struct EncodingFunctionSet {
	EncodingFunctionSet() {};
	static void Initialize(DBConfig &config);
	mutex lock;
	case_insensitive_map_t<EncodingFunction> functions;
};

} // namespace goose
