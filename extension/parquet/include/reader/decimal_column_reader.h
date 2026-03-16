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
