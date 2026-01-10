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

#include <goose/function/function_set.h>
#include <goose/common/operator/multiply.h>

namespace goose {

struct ToSecondsOperator {
	template <class TA, class TR>
	static inline TR Operation(TA input) {
		interval_t result;
		result.months = 0;
		result.days = 0;
		if (!TryMultiplyOperator::Operation<TA, int64_t, int64_t>(input, Interval::MICROS_PER_SEC, result.micros)) {
			throw OutOfRangeException("Interval value %s seconds out of range", NumericHelper::ToString(input));
		}
		return result;
	}
};

} // namespace goose
