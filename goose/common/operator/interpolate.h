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

#include <goose/common/numeric_utils.h>
#include <goose/common/types/interval.h>
#include <goose/common/types/timestamp.h>

namespace goose {

//	Linear interpolation between two values
struct InterpolateOperator {
	template <typename TARGET_TYPE>
	static inline TARGET_TYPE Operation(const TARGET_TYPE &lo, const double d, const TARGET_TYPE &hi) {
		const auto delta = static_cast<double>(hi - lo);
		return LossyNumericCast<TARGET_TYPE>(lo + static_cast<TARGET_TYPE>(delta * d));
	}
};

template <>
double InterpolateOperator::Operation(const double &lo, const double d, const double &hi);
template <>
dtime_t InterpolateOperator::Operation(const dtime_t &lo, const double d, const dtime_t &hi);
template <>
timestamp_t InterpolateOperator::Operation(const timestamp_t &lo, const double d, const timestamp_t &hi);
template <>
hugeint_t InterpolateOperator::Operation(const hugeint_t &lo, const double d, const hugeint_t &hi);
template <>
uhugeint_t InterpolateOperator::Operation(const uhugeint_t &lo, const double d, const uhugeint_t &hi);
template <>
interval_t InterpolateOperator::Operation(const interval_t &lo, const double d, const interval_t &hi);

} // namespace goose
