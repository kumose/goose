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
#include <goose/common/type_util.h>
#include <goose/common/exception.h>
#include <goose/common/types/cast_helpers.h>

namespace goose {

struct AddOperator {
	template <class TA, class TB, class TR>
	static inline TR Operation(TA left, TB right) {
		return left + right;
	}
};

template <>
float AddOperator::Operation(float left, float right);
template <>
double AddOperator::Operation(double left, double right);
template <>
date_t AddOperator::Operation(date_t left, int32_t right);
template <>
date_t AddOperator::Operation(int32_t left, date_t right);
template <>
timestamp_t AddOperator::Operation(date_t left, dtime_t right);
template <>
timestamp_t AddOperator::Operation(dtime_t left, date_t right);
template <>
timestamp_t AddOperator::Operation(date_t left, dtime_tz_t right);
template <>
timestamp_t AddOperator::Operation(dtime_tz_t left, date_t right);
template <>
interval_t AddOperator::Operation(interval_t left, interval_t right);
template <>
timestamp_t AddOperator::Operation(date_t left, interval_t right);
template <>
timestamp_t AddOperator::Operation(interval_t left, date_t right);
template <>
timestamp_t AddOperator::Operation(timestamp_t left, interval_t right);
template <>
timestamp_t AddOperator::Operation(interval_t left, timestamp_t right);

struct TryAddOperator {
	template <class TA, class TB, class TR>
	static inline bool Operation(TA left, TB right, TR &result) {
		throw InternalException("Unimplemented type for TryAddOperator");
	}
};

template <>
bool TryAddOperator::Operation(uint8_t left, uint8_t right, uint8_t &result);
template <>
bool TryAddOperator::Operation(uint16_t left, uint16_t right, uint16_t &result);
template <>
bool TryAddOperator::Operation(uint32_t left, uint32_t right, uint32_t &result);
template <>
bool TryAddOperator::Operation(uint64_t left, uint64_t right, uint64_t &result);
template <>
bool TryAddOperator::Operation(date_t left, int32_t right, date_t &result);

template <>
bool TryAddOperator::Operation(int8_t left, int8_t right, int8_t &result);
template <>
bool TryAddOperator::Operation(int16_t left, int16_t right, int16_t &result);
template <>
bool TryAddOperator::Operation(int32_t left, int32_t right, int32_t &result);
template <>
GOOSE_API bool TryAddOperator::Operation(int64_t left, int64_t right, int64_t &result);
template <>
bool TryAddOperator::Operation(uhugeint_t left, uhugeint_t right, uhugeint_t &result);
template <>
bool TryAddOperator::Operation(hugeint_t left, hugeint_t right, hugeint_t &result);

struct AddOperatorOverflowCheck {
	template <class TA, class TB, class TR>
	static inline TR Operation(TA left, TB right) {
		TR result;
		if (!TryAddOperator::Operation(left, right, result)) {
			throw OutOfRangeException("Overflow in addition of %s (%s + %s)!", TypeIdToString(GetTypeId<TA>()),
			                          NumericHelper::ToString(left), NumericHelper::ToString(right));
		}
		return result;
	}
};

struct TryDecimalAdd {
	template <class TA, class TB, class TR>
	static inline bool Operation(TA left, TB right, TR &result) {
		throw InternalException("Unimplemented type for TryDecimalAdd");
	}
};

template <>
bool TryDecimalAdd::Operation(int16_t left, int16_t right, int16_t &result);
template <>
bool TryDecimalAdd::Operation(int32_t left, int32_t right, int32_t &result);
template <>
bool TryDecimalAdd::Operation(int64_t left, int64_t right, int64_t &result);
template <>
bool TryDecimalAdd::Operation(hugeint_t left, hugeint_t right, hugeint_t &result);

struct DecimalAddOverflowCheck {
	template <class TA, class TB, class TR>
	static inline TR Operation(TA left, TB right) {
		TR result;
		if (!TryDecimalAdd::Operation<TA, TB, TR>(left, right, result)) {
			throw OutOfRangeException("Overflow in addition of DECIMAL(18) (%d + %d). You might want to add an "
			                          "explicit cast to a bigger decimal.",
			                          left, right);
		}
		return result;
	}
};

template <>
hugeint_t DecimalAddOverflowCheck::Operation(hugeint_t left, hugeint_t right);

struct AddTimeOperator {
	template <class TA, class TB, class TR>
	static inline TR Operation(TA left, TB right);
};

template <>
dtime_t AddTimeOperator::Operation(dtime_t left, interval_t right);
template <>
dtime_t AddTimeOperator::Operation(interval_t left, dtime_t right);

template <>
dtime_tz_t AddTimeOperator::Operation(dtime_tz_t left, interval_t right);
template <>
dtime_tz_t AddTimeOperator::Operation(interval_t left, dtime_tz_t right);

} // namespace goose
