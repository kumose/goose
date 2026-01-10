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

#include <goose/storage/compression/chimp/algorithm/chimp128.h>
#include <goose/common/assert.h>
#include <goose/common/exception.h>
#include <goose/common/helper.h>
#include <goose/common/limits.h>
#include <goose/common/types/validity_mask.h>
#include <goose/function/compression_function.h>

namespace goose {

using byte_index_t = uint32_t;

template <class T>
struct ChimpType {};

template <>
struct ChimpType<double> {
	using TYPE = uint64_t;
};

template <>
struct ChimpType<float> {
	using TYPE = uint32_t;
};

class ChimpPrimitives {
public:
	static constexpr uint32_t CHIMP_SEQUENCE_SIZE = 1024;
	static constexpr uint8_t MAX_BYTES_PER_VALUE = sizeof(double) + 1; // extra wiggle room
	static constexpr uint8_t HEADER_SIZE = sizeof(uint32_t);
	static constexpr uint8_t FLAG_BIT_SIZE = 2;
	static constexpr uint32_t LEADING_ZERO_BLOCK_BUFFERSIZE = 1 + (CHIMP_SEQUENCE_SIZE / 8) * 3;
};

//! Where all the magic happens
template <class T, bool EMPTY>
struct ChimpState {
public:
	using CHIMP_TYPE = typename ChimpType<T>::TYPE;

	ChimpState() : chimp() {
	}
	Chimp128CompressionState<CHIMP_TYPE, EMPTY> chimp;

public:
	void AssignDataBuffer(uint8_t *data_out) {
		chimp.output.SetStream(data_out);
	}

	void AssignFlagBuffer(uint8_t *flag_out) {
		chimp.flag_buffer.SetBuffer(flag_out);
	}

	void AssignPackedDataBuffer(uint16_t *packed_data_out) {
		chimp.packed_data_buffer.SetBuffer(packed_data_out);
	}

	void AssignLeadingZeroBuffer(uint8_t *leading_zero_out) {
		chimp.leading_zero_buffer.SetBuffer(leading_zero_out);
	}

	void Flush() {
		chimp.output.Flush();
	}
};

} // namespace goose
