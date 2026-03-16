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

#include <cstdint>

namespace goose {

//! a saner size_t for loop indices etc
typedef uint64_t idx_t;

//! The type used for row identifiers
typedef int64_t row_t;

//! The type used for hashes
typedef uint64_t hash_t;

//! data pointers
typedef uint8_t data_t;
typedef data_t *data_ptr_t;
typedef const data_t *const_data_ptr_t;

//! Type used for the selection vector
typedef uint32_t sel_t;
//! Type used for transaction timestamps
typedef idx_t transaction_t;

//! Type used to identify connections
typedef idx_t connection_t;

//! Type used for column identifiers
typedef idx_t column_t;
//! Type used for storage (column) identifiers
typedef idx_t storage_t;

template <class SRC>
data_ptr_t data_ptr_cast(SRC *src) { // NOLINT: naming
	return reinterpret_cast<data_ptr_t>(src);
}

template <class SRC>
const_data_ptr_t const_data_ptr_cast(const SRC *src) { // NOLINT: naming
	return reinterpret_cast<const_data_ptr_t>(src);
}

template <class SRC>
char *char_ptr_cast(SRC *src) { // NOLINT: naming
	return reinterpret_cast<char *>(src);
}

template <class SRC>
const char *const_char_ptr_cast(const SRC *src) { // NOLINT: naming
	return reinterpret_cast<const char *>(src);
}

template <class SRC>
const unsigned char *const_uchar_ptr_cast(const SRC *src) { // NOLINT: naming
	return reinterpret_cast<const unsigned char *>(src);
}

template <class SRC>
uintptr_t CastPointerToValue(SRC *src) {
	return reinterpret_cast<uintptr_t>(src);
}

template <class SRC>
uint64_t cast_pointer_to_uint64(SRC *src) {
	return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src));
}

template <class SRC = data_t>
SRC *cast_uint64_to_pointer(uint64_t value) {
	return reinterpret_cast<SRC *>(static_cast<uintptr_t>(value));
}

} // namespace goose
