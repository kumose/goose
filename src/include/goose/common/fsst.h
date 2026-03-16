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

#include <goose/common/typedefs.h>
#include <goose/common/vector.h>
#include <goose/storage/string_uncompressed.h>
#include <goose/common/types/value.h>
#include <goose/common/fsst.h>
#include <xfsst/fsst.h>
#include <goose/common/types/vector_buffer.h>

namespace goose {

class Value;
class Vector;
struct string_t;

class FSSTPrimitives {
private:
	// This allows us to decode FSST strings efficiently directly into a string_t (if inlined)
	// The decode will overflow a bit into "extra_space", but "str" will contain the full string
	struct StringWithExtraSpace {
		string_t str;
		// INLINE_BYTES instead of INLINE_LENGTH so this isn't 0-length array when building with GOOSE_DEBUG_NO_INLINE
		uint64_t extra_space[string_t::INLINE_BYTES];
	};

public:
	static string_t DecompressValue(void *goose_fsst_decoder, VectorStringBuffer &str_buffer,
	                                const char *compressed_string, const idx_t compressed_string_len) {
		const auto max_uncompressed_length = compressed_string_len * 8;
		const auto fsst_decoder = static_cast<xfsst_decoder_t *>(goose_fsst_decoder);
		const auto compressed_string_ptr = (const unsigned char *)compressed_string; // NOLINT
		const auto target_ptr = str_buffer.AllocateShrinkableBuffer(max_uncompressed_length);
		const auto decompressed_string_size = xfsst_decompress(
		    fsst_decoder, compressed_string_len, compressed_string_ptr, max_uncompressed_length, target_ptr);
		return str_buffer.FinalizeShrinkableBuffer(target_ptr, max_uncompressed_length, decompressed_string_size);
	}
	static string_t DecompressInlinedValue(void *goose_fsst_decoder, const char *compressed_string,
	                                       const idx_t compressed_string_len) {
		const auto fsst_decoder = static_cast<xfsst_decoder_t *>(goose_fsst_decoder);
		const auto compressed_string_ptr = (const unsigned char *)compressed_string; // NOLINT
		StringWithExtraSpace result;
		const auto target_ptr = (unsigned char *)result.str.GetPrefixWriteable(); // NOLINT
		const auto decompressed_string_size =
		    xfsst_decompress(fsst_decoder, compressed_string_len, compressed_string_ptr,
		                           string_t::INLINE_LENGTH + sizeof(StringWithExtraSpace::extra_space), target_ptr);
		if (decompressed_string_size > string_t::INLINE_LENGTH) {
			throw IOException("Corrupt database file: decoded FSST string of >=%llu bytes (should be <=%llu bytes)",
			                  decompressed_string_size, string_t::INLINE_LENGTH);
		}
		D_ASSERT(decompressed_string_size <= string_t::INLINE_LENGTH);
		result.str.SetSizeAndFinalize(UnsafeNumericCast<uint32_t>(decompressed_string_size), string_t::INLINE_LENGTH);
		return result.str;
	}
	static string DecompressValue(void *goose_fsst_decoder, const char *compressed_string,
	                              const idx_t compressed_string_len, vector<unsigned char> &decompress_buffer);
};
} // namespace goose
