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
#include "parquet_decimal_utils.h"

namespace goose {

template <class GOOSE_PHYSICAL_TYPE, bool FIXED_LENGTH>
struct DecimalParquetValueConversion {
	template <bool CHECKED>
	static GOOSE_PHYSICAL_TYPE PlainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		idx_t byte_len;
		if (FIXED_LENGTH) {
			byte_len = reader.Schema().type_length;
		} else {
			byte_len = plain_data.read<uint32_t>();
		}
		plain_data.available(byte_len);
		auto res = ParquetDecimalUtils::ReadDecimalValue<GOOSE_PHYSICAL_TYPE>(const_data_ptr_cast(plain_data.ptr),
		                                                                       byte_len, reader.Schema());

		plain_data.inc(byte_len);
		return res;
	}

	template <bool CHECKED>
	static void PlainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		uint32_t decimal_len = FIXED_LENGTH ? reader.Schema().type_length : plain_data.read<uint32_t>();
		plain_data.inc(decimal_len);
	}

	static bool PlainAvailable(const ByteBuffer &plain_data, const idx_t count) {
		return true;
	}

	static idx_t PlainConstantSize() {
		return 0;
	}
};

template <class GOOSE_PHYSICAL_TYPE, bool FIXED_LENGTH>
class DecimalColumnReader
    : public TemplatedColumnReader<GOOSE_PHYSICAL_TYPE,
                                   DecimalParquetValueConversion<GOOSE_PHYSICAL_TYPE, FIXED_LENGTH>> {
	using BaseType =
	    TemplatedColumnReader<GOOSE_PHYSICAL_TYPE, DecimalParquetValueConversion<GOOSE_PHYSICAL_TYPE, FIXED_LENGTH>>;

public:
	DecimalColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema)
	    : TemplatedColumnReader<GOOSE_PHYSICAL_TYPE,
	                            DecimalParquetValueConversion<GOOSE_PHYSICAL_TYPE, FIXED_LENGTH>>(reader, schema) {
	}
};

} // namespace goose
