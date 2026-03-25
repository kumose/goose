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
