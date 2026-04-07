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

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <goose/common/operator/comparison_operators.h>

namespace goose {

struct Min {
	template <class T>
	static inline T Operation(T left, T right) {
		return LessThan::Operation(left, right) ? left : right;
	}
};

struct Max {
	template <class T>
	static inline T Operation(T left, T right) {
		return GreaterThan::Operation(left, right) ? left : right;
	}
};

} // namespace goose
