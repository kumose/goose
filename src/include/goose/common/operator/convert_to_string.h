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
