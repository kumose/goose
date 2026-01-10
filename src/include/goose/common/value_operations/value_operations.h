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
