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
#include <goose/common/types/string_type.h>
#include <goose/common/types.h>
#include <goose/common/types/interval.h>
#include <goose/common/limits.h>
#include <goose/common/windows_undefs.h>

#include <limits>
#include <cstring>
#include <cmath>

namespace goose {

//! Placeholder to insert in Vectors or to use for hashing NULLs
template <class T>
inline T NullValue() {
	return std::numeric_limits<T>::min();
}

constexpr const char str_nil[2] = {'\200', '\0'};

template <>
inline const char *NullValue() {
	D_ASSERT(str_nil[0] == '\200' && str_nil[1] == '\0');
	return str_nil;
}

template <>
inline string_t NullValue() {
	return string_t(NullValue<const char *>());
}

template <>
inline char *NullValue() {
	return (char *)NullValue<const char *>(); // NOLINT
}

template <>
inline string NullValue() {
	return string(NullValue<const char *>());
}

template <>
inline interval_t NullValue() {
	interval_t null_value;
	null_value.days = NullValue<int32_t>();
	null_value.months = NullValue<int32_t>();
	null_value.micros = NullValue<int64_t>();
	return null_value;
}

template <>
inline hugeint_t NullValue() {
	hugeint_t min;
	min.lower = 0;
	min.upper = std::numeric_limits<int64_t>::min();
	return min;
}

template <>
inline float NullValue() {
	return NAN;
}

template <>
inline double NullValue() {
	return NAN;
}

} // namespace goose
