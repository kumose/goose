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
