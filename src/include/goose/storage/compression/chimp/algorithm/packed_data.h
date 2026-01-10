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
#include <goose/storage/compression/chimp/algorithm/chimp_utils.h>
#include <goose/common/assert.h>
#include <goose/goose-c.h>

namespace goose {

struct UnpackedData {
	uint8_t leading_zero;
	uint8_t significant_bits;
	uint8_t index;
};

template <class CHIMP_TYPE>
struct PackedDataUtils {
private:
	static constexpr uint8_t INDEX_BITS_SIZE = 7;
	static constexpr uint8_t LEADING_BITS_SIZE = 3;

	static constexpr uint8_t INDEX_MASK = ((uint8_t)1 << INDEX_BITS_SIZE) - 1;
	static constexpr uint8_t LEADING_MASK = ((uint8_t)1 << LEADING_BITS_SIZE) - 1;

	static constexpr uint8_t INDEX_SHIFT_AMOUNT = (sizeof(uint16_t) * 8) - INDEX_BITS_SIZE;
	static constexpr uint8_t LEADING_SHIFT_AMOUNT = INDEX_SHIFT_AMOUNT - LEADING_BITS_SIZE;

public:
	//|----------------|	//! packed_data(16) bits
	// IIIIIII				//! Index (7 bits, shifted by 9)
	//        LLL			//! LeadingZeros (3 bits, shifted by 6)
	//           SSSSSS 	//! SignificantBits (6 bits)
	static inline void Unpack(uint16_t packed_data, UnpackedData &dest) {
		dest.index = packed_data >> INDEX_SHIFT_AMOUNT & INDEX_MASK;
		dest.leading_zero = packed_data >> LEADING_SHIFT_AMOUNT & LEADING_MASK;
		dest.significant_bits = packed_data & SignificantBits<CHIMP_TYPE>::mask;
		//  Verify that combined, this is not bigger than the full size of the type
		D_ASSERT(dest.significant_bits + dest.leading_zero <= (sizeof(CHIMP_TYPE) * 8));
	}

	static inline uint16_t Pack(uint8_t index, uint8_t leading_zero, uint8_t significant_bits) {
		static constexpr uint8_t BIT_SIZE = (sizeof(CHIMP_TYPE) * 8);

		uint16_t result = 0;
		result += ((uint32_t)BIT_SIZE << 3) * (ChimpConstants::BUFFER_SIZE + index);
		result += BIT_SIZE * (leading_zero & 7);
		if (BIT_SIZE == 32) {
			// Shift the result by 1 to occupy the 16th bit
			result <<= 1;
		}
		result += (significant_bits & 63);

		return result;
	}
};

template <bool EMPTY>
struct PackedDataBuffer {
public:
	PackedDataBuffer() : index(0), buffer(nullptr) {
	}

public:
	void SetBuffer(uint16_t *buffer) {
		this->buffer = buffer;
		this->index = 0;
	}

	void Reset() {
		this->index = 0;
	}

	inline void Insert(uint16_t packed_data) {
		if (!EMPTY) {
			buffer[index] = packed_data;
		}
		index++;
	}

	idx_t index;
	uint16_t *buffer;
};

} // namespace goose
