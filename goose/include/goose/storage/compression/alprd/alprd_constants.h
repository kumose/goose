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

#include <goose/common/limits.h>

namespace goose {

class AlpRDConstants {
public:
	static constexpr uint32_t ALP_VECTOR_SIZE = 1024;

	static constexpr uint8_t MAX_DICTIONARY_BIT_WIDTH = 3;
	static constexpr uint8_t MAX_DICTIONARY_SIZE = (1 << MAX_DICTIONARY_BIT_WIDTH); // 8
	static constexpr uint8_t CUTTING_LIMIT = 16;
	static constexpr uint8_t DICTIONARY_ELEMENT_SIZE = sizeof(uint16_t);
	static constexpr uint8_t MAX_DICTIONARY_SIZE_BYTES = MAX_DICTIONARY_SIZE * DICTIONARY_ELEMENT_SIZE;

	static constexpr uint8_t EXCEPTION_SIZE = sizeof(uint16_t);
	static constexpr uint8_t METADATA_POINTER_SIZE = sizeof(uint32_t);
	//! exceptions_count can store the UNCOMPRESSED_MODE_SENTINEL value
	static constexpr uint8_t EXCEPTIONS_COUNT_SIZE = sizeof(uint16_t);
	static constexpr uint16_t UNCOMPRESSED_MODE_SENTINEL = std::numeric_limits<uint16_t>::max();
	static constexpr uint8_t EXCEPTION_POSITION_SIZE = sizeof(uint16_t);
	static constexpr uint8_t RIGHT_BIT_WIDTH_SIZE = sizeof(uint8_t);
	static constexpr uint8_t LEFT_BIT_WIDTH_SIZE = sizeof(uint8_t);
	static constexpr uint8_t N_DICTIONARY_ELEMENTS_SIZE = sizeof(uint8_t);
	static constexpr uint8_t HEADER_SIZE =
	    METADATA_POINTER_SIZE + RIGHT_BIT_WIDTH_SIZE + LEFT_BIT_WIDTH_SIZE +
	    N_DICTIONARY_ELEMENTS_SIZE; // Pointer to metadata + Right BW + Left BW + Dict Elems
};

} // namespace goose
