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
