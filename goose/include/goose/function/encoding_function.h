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
