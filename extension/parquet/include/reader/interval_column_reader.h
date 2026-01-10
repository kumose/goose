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

#include "column_reader.h"
#include "reader/templated_column_reader.h"
#include "parquet_reader.h"

namespace goose {

//===--------------------------------------------------------------------===//
// Interval Column Reader
//===--------------------------------------------------------------------===//
struct IntervalValueConversion {
	static constexpr const idx_t PARQUET_INTERVAL_SIZE = 12;

	static interval_t ReadParquetInterval(const_data_ptr_t input) {
		interval_t result;
		result.months = Load<int32_t>(input);
		result.days = Load<int32_t>(input + sizeof(uint32_t));
		result.micros = int64_t(Load<uint32_t>(input + sizeof(uint32_t) * 2)) * 1000;
		return result;
	}

	template <bool CHECKED>
	static interval_t PlainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			plain_data.available(PARQUET_INTERVAL_SIZE);
		}
		auto res = ReadParquetInterval(const_data_ptr_cast(plain_data.ptr));
		plain_data.unsafe_inc(PARQUET_INTERVAL_SIZE);
		return res;
	}

	template <bool CHECKED>
	static void PlainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			plain_data.inc(PARQUET_INTERVAL_SIZE);
		} else {
			plain_data.unsafe_inc(PARQUET_INTERVAL_SIZE);
		}
	}

	static bool PlainAvailable(const ByteBuffer &plain_data, const idx_t count) {
		return plain_data.check_available(count * PARQUET_INTERVAL_SIZE);
	}

	static idx_t PlainConstantSize() {
		return 0;
	}
};

class IntervalColumnReader : public TemplatedColumnReader<interval_t, IntervalValueConversion> {
public:
	IntervalColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema)
	    : TemplatedColumnReader<interval_t, IntervalValueConversion>(reader, schema) {
	}
};

} // namespace goose
