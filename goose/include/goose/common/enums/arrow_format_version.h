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
