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

#include <goose/goose.h>
#include "parquet_dbp_decoder.h"
#include "resizable_buffer.h"

namespace goose {
class ColumnReader;

class DeltaLengthByteArrayDecoder {
public:
	explicit DeltaLengthByteArrayDecoder(ColumnReader &reader);

public:
	void InitializePage();

	void Read(shared_ptr<ResizeableBuffer> &block, uint8_t *defines, idx_t read_count, Vector &result,
	          idx_t result_offset);
	void Skip(uint8_t *defines, idx_t skip_count);

private:
	template <bool HAS_DEFINES, bool VALIDATE_INDIVIDUAL_STRINGS>
	void ReadInternal(shared_ptr<ResizeableBuffer> &block, uint8_t *defines, idx_t read_count, Vector &result,
	                  idx_t result_offset);
	template <bool HAS_DEFINES>
	void SkipInternal(uint8_t *defines, idx_t skip_count);

private:
	ColumnReader &reader;
	ResizeableBuffer &length_buffer;
	idx_t byte_array_count = 0;
	idx_t length_idx;
};

} // namespace goose
