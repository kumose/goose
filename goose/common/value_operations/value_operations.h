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

#include <goose/common/types/value.h>

namespace goose {

struct ValueOperations {
	//===--------------------------------------------------------------------===//
	// Comparison Operations
	//===--------------------------------------------------------------------===//
	// A == B
	static bool Equals(const Value &left, const Value &right);
	// A != B
	static bool NotEquals(const Value &left, const Value &right);
	// A > B
	static bool GreaterThan(const Value &left, const Value &right);
	// A >= B
	static bool GreaterThanEquals(const Value &left, const Value &right);
	// A < B
	static bool LessThan(const Value &left, const Value &right);
	// A <= B
	static bool LessThanEquals(const Value &left, const Value &right);
	//===--------------------------------------------------------------------===//
	// Distinction Operations
	//===--------------------------------------------------------------------===//
	// A == B, NULLs equal
	static bool NotDistinctFrom(const Value &left, const Value &right);
	// A != B, NULLs equal
	static bool DistinctFrom(const Value &left, const Value &right);
	// A > B, NULLs last
	static bool DistinctGreaterThan(const Value &left, const Value &right);
	// A >= B, NULLs last
	static bool DistinctGreaterThanEquals(const Value &left, const Value &right);
	// A < B, NULLs last
	static bool DistinctLessThan(const Value &left, const Value &right);
	// A <= B, NULLs last
	static bool DistinctLessThanEquals(const Value &left, const Value &right);
};
} // namespace goose
