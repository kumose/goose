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
