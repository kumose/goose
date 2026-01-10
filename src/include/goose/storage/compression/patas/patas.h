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

#include <goose/storage/compression/patas/algorithm/patas.h>
#include <goose/common/assert.h>
#include <goose/common/exception.h>
#include <goose/common/helper.h>
#include <goose/common/limits.h>
#include <goose/function/compression_function.h>

namespace goose {

using byte_index_t = uint32_t;

//! FIXME: replace ChimpType with this
template <class T>
struct FloatingToExact {};

template <>
struct FloatingToExact<double> {
	using TYPE = uint64_t;
};

template <>
struct FloatingToExact<float> {
	using TYPE = uint32_t;
};

template <class T, bool EMPTY>
struct PatasState {
public:
	using EXACT_TYPE = typename FloatingToExact<T>::TYPE;

	explicit PatasState(void *state_p = nullptr) : data_ptr(state_p), patas_state() {
	}
	//! The Compress/Analyze State
	void *data_ptr;
	patas::PatasCompressionState<EXACT_TYPE, EMPTY> patas_state;

public:
	void AssignDataBuffer(uint8_t *data_out) {
		patas_state.SetOutputBuffer(data_out);
	}

	template <class OP>
	bool Update(T uncompressed_value, bool is_valid) {
		OP::template Operation<T>(uncompressed_value, is_valid, data_ptr);
		return true;
	}
};

} // namespace goose
