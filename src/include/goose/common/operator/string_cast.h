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

#include <goose/common/common.h>
#include <goose/common/exception.h>
#include <goose/common/types.h>
#include <goose/common/types/date.h>
#include <goose/common/types/string_type.h>
#include <goose/common/types/timestamp.h>

namespace goose {

struct date_t;
struct timestamp_t;
struct timestamp_ns_t;

//! StringCast
class Vector;

struct StringCast {
	template <class SRC>
	static inline string_t Operation(SRC input, Vector &result) {
		throw NotImplementedException("Unimplemented type for string cast!");
	}
};

template <>
GOOSE_API goose::string_t StringCast::Operation(bool input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(int8_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(int16_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(int32_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(int64_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(uint8_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(uint16_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(uint32_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(uint64_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(hugeint_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(uhugeint_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(float input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(double input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(interval_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(goose::string_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(date_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(dtime_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(dtime_ns_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(timestamp_t input, Vector &result);
template <>
GOOSE_API goose::string_t StringCast::Operation(timestamp_ns_t input, Vector &result);

//! Temporary casting for Time Zone types. TODO: turn casting into functions.
struct StringCastTZ {
	template <typename SRC>
	static inline string_t Operation(SRC input, Vector &vector) {
		return StringCast::Operation(input, vector);
	}
};

template <>
goose::string_t StringCastTZ::Operation(date_t input, Vector &result);
template <>
goose::string_t StringCastTZ::Operation(dtime_tz_t input, Vector &result);
template <>
goose::string_t StringCastTZ::Operation(timestamp_t input, Vector &result);

} // namespace goose
