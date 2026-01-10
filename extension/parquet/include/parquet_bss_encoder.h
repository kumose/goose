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

#include "decode_utils.h"

namespace goose {

class BssEncoder {
public:
	explicit BssEncoder(const idx_t total_value_count_p, const idx_t bit_width_p)
	    : total_value_count(total_value_count_p), bit_width(bit_width_p), count(0) {
	}

public:
	void BeginWrite(Allocator &allocator) {
		buffer = allocator.Allocate(total_value_count * bit_width + 1);
	}

	template <class T>
	void WriteValue(const T &value) {
		D_ASSERT(sizeof(T) == bit_width);
		for (idx_t i = 0; i < sizeof(T); i++) {
			buffer.get()[i * total_value_count + count] = reinterpret_cast<const_data_ptr_t>(&value)[i];
		}
		count++;
	}

	void FinishWrite(WriteStream &writer) {
		writer.WriteData(buffer.get(), total_value_count * bit_width);
	}

private:
	const idx_t total_value_count;
	const idx_t bit_width;

	idx_t count;
	AllocatedData buffer;
};

} // namespace goose
