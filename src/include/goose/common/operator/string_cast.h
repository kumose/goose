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
