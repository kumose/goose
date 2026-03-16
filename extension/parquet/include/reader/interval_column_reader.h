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
