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
#include <goose/common/helper.h>

namespace goose {

template <class VALUE_TYPE>
struct TemplatedParquetValueConversion {
	template <bool CHECKED>
	static VALUE_TYPE PlainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			return plain_data.read<VALUE_TYPE>();
		} else {
			return plain_data.unsafe_read<VALUE_TYPE>();
		}
	}

	template <bool CHECKED>
	static void PlainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			plain_data.inc(sizeof(VALUE_TYPE));
		} else {
			plain_data.unsafe_inc(sizeof(VALUE_TYPE));
		}
	}

	static bool PlainAvailable(const ByteBuffer &plain_data, const idx_t count) {
		return plain_data.check_available(count * sizeof(VALUE_TYPE));
	}

	static idx_t PlainConstantSize() {
		return sizeof(VALUE_TYPE);
	}
};

template <class VALUE_TYPE, class VALUE_CONVERSION>
class TemplatedColumnReader : public ColumnReader {
public:
	static constexpr const PhysicalType TYPE = PhysicalType::INVALID;

public:
	TemplatedColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema) : ColumnReader(reader, schema) {
	}

	shared_ptr<ResizeableBuffer> dict;

public:
	void AllocateDict(idx_t size) {
		if (!dict) {
			dict = make_shared_ptr<ResizeableBuffer>(GetAllocator(), size);
		} else {
			dict->resize(GetAllocator(), size);
		}
	}

	void Plain(ByteBuffer &plain_data, uint8_t *defines, uint64_t num_values, idx_t result_offset,
	           Vector &result) override {
		PlainTemplated<VALUE_TYPE, VALUE_CONVERSION>(plain_data, defines, num_values, result_offset, result);
	}

	void PlainSkip(ByteBuffer &plain_data, uint8_t *defines, idx_t num_values) override {
		PlainSkipTemplated<VALUE_CONVERSION>(plain_data, defines, num_values);
	}

	bool SupportsDirectFilter() const override {
		return true;
	}
};

template <class PARQUET_PHYSICAL_TYPE, class GOOSE_PHYSICAL_TYPE,
          GOOSE_PHYSICAL_TYPE (*FUNC)(const PARQUET_PHYSICAL_TYPE &input)>
struct CallbackParquetValueConversion {
	template <bool CHECKED>
	static GOOSE_PHYSICAL_TYPE PlainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			return FUNC(plain_data.read<PARQUET_PHYSICAL_TYPE>());
		} else {
			return FUNC(plain_data.unsafe_read<PARQUET_PHYSICAL_TYPE>());
		}
	}

	template <bool CHECKED>
	static void PlainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		if (CHECKED) {
			plain_data.inc(sizeof(PARQUET_PHYSICAL_TYPE));
		} else {
			plain_data.unsafe_inc(sizeof(PARQUET_PHYSICAL_TYPE));
		}
	}

	static bool PlainAvailable(const ByteBuffer &plain_data, const idx_t count) {
		return plain_data.check_available(count * sizeof(PARQUET_PHYSICAL_TYPE));
	}

	static idx_t PlainConstantSize() {
		return 0;
	}
};

} // namespace goose
