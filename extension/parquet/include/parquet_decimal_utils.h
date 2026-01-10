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
