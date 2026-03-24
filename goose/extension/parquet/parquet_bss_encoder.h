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

#include <goose/extension/parquet/decode_utils.h>

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
