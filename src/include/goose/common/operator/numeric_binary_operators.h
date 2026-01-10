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

#include <goose/common/assert.h>
#include <cmath>

namespace goose {

struct DivideOperator {
	template <class TA, class TB, class TR>
	static inline TR Operation(TA left, TB right) {
		D_ASSERT(right != 0); // this should be checked before!
		return left / right;
	}
};

struct ModuloOperator {
	template <class TA, class TB, class TR>
	static inline TR Operation(TA left, TB right) {
		D_ASSERT(right != 0);
		return left % right;
	}
};

template <>
float DivideOperator::Operation(float left, float right);
template <>
double DivideOperator::Operation(double left, double right);
template <>
hugeint_t DivideOperator::Operation(hugeint_t left, hugeint_t right);
template <>
interval_t DivideOperator::Operation(interval_t left, int64_t right);

template <>
float ModuloOperator::Operation(float left, float right);
template <>
double ModuloOperator::Operation(double left, double right);
template <>
hugeint_t ModuloOperator::Operation(hugeint_t left, hugeint_t right);

} // namespace goose
