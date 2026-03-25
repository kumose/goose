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
