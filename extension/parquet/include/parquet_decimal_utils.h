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

namespace goose {

class ParquetDecimalUtils {
public:
	template <class PHYSICAL_TYPE>
	static PHYSICAL_TYPE ReadDecimalValue(const_data_ptr_t pointer, idx_t size, const ParquetColumnSchema &) {
		PHYSICAL_TYPE res = 0;

		auto res_ptr = (uint8_t *)&res;
		bool positive = (*pointer & 0x80) == 0;

		// numbers are stored as two's complement so some muckery is required
		for (idx_t i = 0; i < MinValue<idx_t>(size, sizeof(PHYSICAL_TYPE)); i++) {
			auto byte = *(pointer + (size - i - 1));
			res_ptr[i] = positive ? byte : byte ^ 0xFF;
		}
		// Verify that there are only 0s here
		if (size > sizeof(PHYSICAL_TYPE)) {
			for (idx_t i = sizeof(PHYSICAL_TYPE); i < size; i++) {
				auto byte = *(pointer + (size - i - 1));
				if (!positive) {
					byte ^= 0xFF;
				}
				if (byte != 0) {
					throw InvalidInputException("Invalid decimal encoding in Parquet file");
				}
			}
		}
		if (!positive) {
			res += 1;
			return -res;
		}
		return res;
	}

	static unique_ptr<ColumnReader> CreateReader(ParquetReader &reader, const ParquetColumnSchema &schema);
};

template <>
double ParquetDecimalUtils::ReadDecimalValue(const_data_ptr_t pointer, idx_t size, const ParquetColumnSchema &schema);

} // namespace goose
