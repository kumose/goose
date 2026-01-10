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
