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

#include <goose/common/types.h>

namespace goose {

template <class PHYSICAL_TYPE>
struct DecimalWidth {};

template <>
struct DecimalWidth<int16_t> {
	static constexpr uint8_t max = 4;
};

template <>
struct DecimalWidth<int32_t> {
	static constexpr uint8_t max = 9;
};

template <>
struct DecimalWidth<int64_t> {
	static constexpr uint8_t max = 18;
};

template <>
struct DecimalWidth<hugeint_t> {
	static constexpr uint8_t max = 38;
};

//! The Decimal class is a static class that holds helper functions for the Decimal type
class Decimal {
public:
	static constexpr uint8_t MAX_WIDTH_INT16 = DecimalWidth<int16_t>::max;
	static constexpr uint8_t MAX_WIDTH_INT32 = DecimalWidth<int32_t>::max;
	static constexpr uint8_t MAX_WIDTH_INT64 = DecimalWidth<int64_t>::max;
	static constexpr uint8_t MAX_WIDTH_INT128 = DecimalWidth<hugeint_t>::max;
	static constexpr uint8_t MAX_WIDTH_DECIMAL = MAX_WIDTH_INT128;

public:
	static string ToString(int16_t value, uint8_t width, uint8_t scale);
	static string ToString(int32_t value, uint8_t width, uint8_t scale);
	static string ToString(int64_t value, uint8_t width, uint8_t scale);
	static string ToString(hugeint_t value, uint8_t width, uint8_t scale);
};
} // namespace goose
