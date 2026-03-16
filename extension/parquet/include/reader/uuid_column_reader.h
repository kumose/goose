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
#include "templated_column_reader.h"
#include "parquet_reader.h"
#include <goose/common/types/uuid.h>

namespace goose {

struct UUIDValueConversion {
	static hugeint_t ReadParquetUUID(const_data_ptr_t input) {
		// Use the utility function from BaseUUID
		return BaseUUID::FromBlob(input);
	}

	template <bool CHECKED>
	static hugeint_t PlainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			plain_data.available(sizeof(hugeint_t));
		}
		auto res = ReadParquetUUID(const_data_ptr_cast(plain_data.ptr));
		plain_data.unsafe_inc(sizeof(hugeint_t));
		return res;
	}

	template <bool CHECKED>
	static void PlainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			plain_data.inc(sizeof(hugeint_t));
		} else {
			plain_data.unsafe_inc(sizeof(hugeint_t));
		}
	}

	static bool PlainAvailable(const ByteBuffer &plain_data, const idx_t count) {
		return plain_data.check_available(count * sizeof(hugeint_t));
	}

	static idx_t PlainConstantSize() {
		return 0;
	}
};

class UUIDColumnReader : public TemplatedColumnReader<hugeint_t, UUIDValueConversion> {
public:
	UUIDColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema)
	    : TemplatedColumnReader<hugeint_t, UUIDValueConversion>(reader, schema) {
	}
};

} // namespace goose
