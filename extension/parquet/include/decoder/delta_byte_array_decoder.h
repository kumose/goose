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

class DeltaByteArrayDecoder {
public:
	explicit DeltaByteArrayDecoder(ColumnReader &reader);

public:
	void InitializePage();

	void Read(uint8_t *defines, idx_t read_count, Vector &result, idx_t result_offset);
	void Skip(uint8_t *defines, idx_t skip_count);

	static void ReadDbpData(Allocator &allocator, ResizeableBuffer &buffer, ResizeableBuffer &result_buffer,
	                        idx_t &value_count);

private:
	ColumnReader &reader;

	//! Decoded data in plain Parquet page format
	shared_ptr<ResizeableBuffer> plain_data;
};

} // namespace goose
