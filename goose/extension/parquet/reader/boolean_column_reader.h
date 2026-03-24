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

namespace goose {

struct BooleanParquetValueConversion;

class BooleanColumnReader : public TemplatedColumnReader<bool, BooleanParquetValueConversion> {
public:
	static constexpr const PhysicalType TYPE = PhysicalType::BOOL;

public:
	BooleanColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema)
	    : TemplatedColumnReader<bool, BooleanParquetValueConversion>(reader, schema), byte_pos(0) {
	}

	uint8_t byte_pos;

	void InitializeRead(idx_t row_group_idx_p, const vector<ColumnChunk> &columns, TProtocol &protocol_p) override {
		byte_pos = 0;
		TemplatedColumnReader<bool, BooleanParquetValueConversion>::InitializeRead(row_group_idx_p, columns,
		                                                                           protocol_p);
	}

	void ResetPage() override {
		byte_pos = 0;
	}
};

struct BooleanParquetValueConversion {
	template <bool CHECKED>
	static bool PlainRead(ByteBuffer &plain_data, ColumnReader &reader) {
		auto &byte_pos = reader.Cast<BooleanColumnReader>().byte_pos;
		bool ret = (*plain_data.ptr >> byte_pos) & 1;
		if (++byte_pos == 8) {
			byte_pos = 0;
			if (CHECKED) {
				plain_data.inc(1);
			} else {
				plain_data.unsafe_inc(1);
			}
		}
		return ret;
	}

	template <bool CHECKED>
	static void PlainSkip(ByteBuffer &plain_data, ColumnReader &reader) {
		PlainRead<CHECKED>(plain_data, reader);
	}

	static bool PlainAvailable(const ByteBuffer &plain_data, const idx_t count) {
		return plain_data.check_available((count + 7) / 8);
	}

	static idx_t PlainConstantSize() {
		return 0;
	}
};

} // namespace goose
