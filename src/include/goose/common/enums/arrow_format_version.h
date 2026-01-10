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

enum class ArrowOffsetSize : uint8_t { REGULAR, LARGE };

enum class ArrowFormatVersion : uint8_t {
	//! Base Version
	V1_0 = 10,
	//! Added 256-bit Decimal type.
	V1_1 = 11,
	//! Added MonthDayNano interval type.
	V1_2 = 12,
	//! Added Run-End Encoded Layout.
	V1_3 = 13,
	//! Added Variable-size Binary View Layout and the associated BinaryView and Utf8View types.
	//! Added ListView Layout and the associated ListView and LargeListView types. Added Variadic buffers.
	V1_4 = 14,
	//! Expanded Decimal type bit widths to allow 32-bit and 64-bit types.
	V1_5 = 15
};

} // namespace goose
