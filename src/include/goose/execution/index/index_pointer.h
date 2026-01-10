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

#include <goose/common/numeric_utils.h>
#include <goose/common/typedefs.h>

namespace goose {

class IndexPointer {
public:
	//! Bit-shifting
	static constexpr idx_t SHIFT_OFFSET = 32;
	static constexpr idx_t SHIFT_METADATA = 56;
	//! AND operations
	static constexpr idx_t AND_OFFSET = 0x0000000000FFFFFF;
	static constexpr idx_t AND_BUFFER_ID = 0x00000000FFFFFFFF;
	static constexpr idx_t AND_METADATA = 0xFF00000000000000;

public:
	//! Constructs an empty IndexPointer
	IndexPointer() : data(0) {};
	//! Constructs an in-memory IndexPointer with a buffer ID and an offset
	IndexPointer(const uint32_t buffer_id, const uint32_t offset) : data(0) {
		auto shifted_offset = UnsafeNumericCast<idx_t>(offset) << SHIFT_OFFSET;
		data += shifted_offset;
		data += buffer_id;
	};

public:
	//! Get data (all 64 bits)
	inline idx_t Get() const {
		return data;
	}
	//! Set data (all 64 bits)
	inline void Set(const idx_t data_p) {
		data = data_p;
	}

	//! Returns false, if the metadata is empty
	inline bool HasMetadata() const {
		return data & AND_METADATA;
	}
	//! Get metadata (Bits 56-63)
	inline uint8_t GetMetadata() const {
		return data >> SHIFT_METADATA;
	}
	//! Set metadata (Bits 56-63)
	inline void SetMetadata(const uint8_t metadata) {
		data &= ~AND_METADATA;
		data |= UnsafeNumericCast<idx_t>(metadata) << SHIFT_METADATA;
	}

	//! Get the offset (Bits 32-55)
	inline idx_t GetOffset() const {
		auto offset = data >> SHIFT_OFFSET;
		return offset & AND_OFFSET;
	}
	//! Get the buffer ID (Bits 0-31)
	inline idx_t GetBufferId() const {
		return data & AND_BUFFER_ID;
	}

	//! Resets the IndexPointer
	inline void Clear() {
		data = 0;
	}

	//! Adds an idx_t to a buffer ID, the rightmost 32 bits of data (Bits 0-31) contain the buffer ID
	inline void IncreaseBufferId(const idx_t summand) {
		data += summand;
	}

	//! Comparison operator
	inline bool operator==(const IndexPointer &ptr) const {
		return data == ptr.data;
	}

private:
	//! Data holds all the information contained in an IndexPointer (64-bit value)
	//! Bit layout:
	//! MSB                                                                        LSB
	//! 63         56 55                      32 31                                  0
	//! +------------+--------------------------+------------------------------------+
	//! |  Metadata  |           Offset         |           Buffer ID                |
	//! |   8 bits   |          24 bits         |            32 bits                 |
	//! +------------+--------------------------+------------------------------------+
	//! NOTE: we do not use bit fields because when using bit fields Windows compiles
	//! the IndexPointer class into 16 bytes instead of the intended 8 bytes, doubling the
	//! space requirements
	//! https://learn.microsoft.com/en-us/cpp/cpp/cpp-bit-fields?view=msvc-170
	idx_t data;
};

static_assert(sizeof(IndexPointer) == sizeof(idx_t), "Invalid size for IndexPointer.");

} // namespace goose
