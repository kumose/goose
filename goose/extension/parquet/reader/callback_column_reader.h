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

#include <goose/extension/parquet/column_reader.h>
#include <goose/extension/parquet/reader/templated_column_reader.h>
#include <goose/extension/parquet/parquet_reader.h>

namespace goose {

template <class PARQUET_PHYSICAL_TYPE, class GOOSE_PHYSICAL_TYPE,
          GOOSE_PHYSICAL_TYPE (*FUNC)(const PARQUET_PHYSICAL_TYPE &input)>
class CallbackColumnReader
    : public TemplatedColumnReader<GOOSE_PHYSICAL_TYPE,
                                   CallbackParquetValueConversion<PARQUET_PHYSICAL_TYPE, GOOSE_PHYSICAL_TYPE, FUNC>> {
	using BaseType =
	    TemplatedColumnReader<GOOSE_PHYSICAL_TYPE,
	                          CallbackParquetValueConversion<PARQUET_PHYSICAL_TYPE, GOOSE_PHYSICAL_TYPE, FUNC>>;

public:
	static constexpr const PhysicalType TYPE = PhysicalType::INVALID;

public:
	CallbackColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema)
	    : TemplatedColumnReader<GOOSE_PHYSICAL_TYPE,
	                            CallbackParquetValueConversion<PARQUET_PHYSICAL_TYPE, GOOSE_PHYSICAL_TYPE, FUNC>>(
	          reader, schema) {
	}

protected:
	void Dictionary(shared_ptr<ResizeableBuffer> dictionary_data, idx_t num_entries) {
		BaseType::AllocateDict(num_entries * sizeof(GOOSE_PHYSICAL_TYPE));
		auto dict_ptr = (GOOSE_PHYSICAL_TYPE *)this->dict->ptr;
		for (idx_t i = 0; i < num_entries; i++) {
			dict_ptr[i] = FUNC(dictionary_data->read<PARQUET_PHYSICAL_TYPE>());
		}
	}
};

} // namespace goose
