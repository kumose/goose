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

#include <goose/common/constants.h>

namespace goose {

enum class MemoryTag : uint8_t {
	BASE_TABLE = 0,
	HASH_TABLE = 1,
	PARQUET_READER = 2,
	CSV_READER = 3,
	ORDER_BY = 4,
	ART_INDEX = 5,
	COLUMN_DATA = 6,
	METADATA = 7,
	OVERFLOW_STRINGS = 8,
	IN_MEMORY_TABLE = 9,
	ALLOCATOR = 10,
	EXTENSION = 11,
	TRANSACTION = 12,
	EXTERNAL_FILE_CACHE = 13,
	WINDOW = 14
};

static constexpr const idx_t MEMORY_TAG_COUNT = 15;

} // namespace goose
