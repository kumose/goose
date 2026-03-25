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
#include <goose/common/types.h>
#include <goose/common/type_util.h>
#include <goose/common/exception.h>

namespace goose {

struct ConvertToString {
	template <class SRC>
	static inline string Operation(SRC input) {
		throw InternalException("Unrecognized type for ConvertToString %s", GetTypeId<SRC>());
	}
};

template <>
GOOSE_API string ConvertToString::Operation(bool input);
template <>
GOOSE_API string ConvertToString::Operation(int8_t input);
template <>
GOOSE_API string ConvertToString::Operation(int16_t input);
template <>
GOOSE_API string ConvertToString::Operation(int32_t input);
template <>
GOOSE_API string ConvertToString::Operation(int64_t input);
template <>
GOOSE_API string ConvertToString::Operation(uint8_t input);
template <>
GOOSE_API string ConvertToString::Operation(uint16_t input);
template <>
GOOSE_API string ConvertToString::Operation(uint32_t input);
template <>
GOOSE_API string ConvertToString::Operation(uint64_t input);
template <>
GOOSE_API string ConvertToString::Operation(hugeint_t input);
template <>
GOOSE_API string ConvertToString::Operation(uhugeint_t input);
template <>
GOOSE_API string ConvertToString::Operation(float input);
template <>
GOOSE_API string ConvertToString::Operation(double input);
template <>
GOOSE_API string ConvertToString::Operation(interval_t input);
template <>
GOOSE_API string ConvertToString::Operation(date_t input);
template <>
GOOSE_API string ConvertToString::Operation(dtime_t input);
template <>
GOOSE_API string ConvertToString::Operation(timestamp_t input);
template <>
GOOSE_API string ConvertToString::Operation(string_t input);

} // namespace goose
