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
