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
