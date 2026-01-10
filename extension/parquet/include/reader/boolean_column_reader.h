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
