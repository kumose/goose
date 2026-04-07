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
#include <goose/common/helper.h>

namespace goose {

class NullColumnReader : public ColumnReader {
public:
	static constexpr const PhysicalType TYPE = PhysicalType::INVALID;

public:
	NullColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema) : ColumnReader(reader, schema) {};

	shared_ptr<ResizeableBuffer> dict;

public:
	void Plain(ByteBuffer &plain_data, uint8_t *defines, uint64_t num_values, idx_t result_offset,
	           Vector &result) override {
		(void)defines;
		(void)plain_data;

		auto &result_mask = FlatVector::Validity(result);
		for (idx_t row_idx = 0; row_idx < num_values; row_idx++) {
			result_mask.SetInvalid(row_idx + result_offset);
		}
	}
};

} // namespace goose
