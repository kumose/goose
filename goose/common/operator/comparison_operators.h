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

#include <goose/common/helper.h>
#include <goose/common/types.h>
#include <goose/common/types/hugeint.h>
#include <goose/common/types/interval.h>
#include <goose/common/types/string_type.h>

#include <cstring>

namespace goose {

//===--------------------------------------------------------------------===//
// Comparison Operations
//===--------------------------------------------------------------------===//
struct Equals {
	template <class T>
	static inline bool Operation(const T &left, const T &right) {
		return left == right;
	}
};
struct NotEquals {
	template <class T>
	static inline bool Operation(const T &left, const T &right) {
		return !Equals::Operation(left, right);
	}
};

struct GreaterThan {
	template <class T>
	static inline bool Operation(const T &left, const T &right) {
		return left > right;
	}
};

struct GreaterThanEquals {
	template <class T>
	static inline bool Operation(const T &left, const T &right) {
		return !GreaterThan::Operation(right, left);
	}
};

struct LessThan {
	template <class T>
	static inline bool Operation(const T &left, const T &right) {
		return GreaterThan::Operation(right, left);
	}
};

struct LessThanEquals {
	template <class T>
	static inline bool Operation(const T &left, const T &right) {
		return !GreaterThan::Operation(left, right);
	}
};

template <>
GOOSE_API bool Equals::Operation(const float &left, const float &right);
template <>
GOOSE_API bool Equals::Operation(const double &left, const double &right);

template <>
GOOSE_API bool GreaterThan::Operation(const float &left, const float &right);
template <>
GOOSE_API bool GreaterThan::Operation(const double &left, const double &right);

template <>
GOOSE_API bool GreaterThanEquals::Operation(const float &left, const float &right);
template <>
GOOSE_API bool GreaterThanEquals::Operation(const double &left, const double &right);

// Distinct semantics are from Postgres record sorting. NULL = NULL and not-NULL < NULL
// Deferring to the non-distinct operations removes the need for further specialisation.
// TODO: To reverse the semantics, swap left_null and right_null for comparisons
struct DistinctFrom {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		if (left_null || right_null) {
			return left_null != right_null;
		}
		return NotEquals::Operation(left, right);
	}
};

struct NotDistinctFrom {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return !DistinctFrom::Operation(left, right, left_null, right_null);
	}
};

struct DistinctGreaterThan {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		if (left_null || right_null) {
			return !right_null;
		}
		return GreaterThan::Operation(left, right);
	}
};

struct DistinctGreaterThanNullsFirst {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return DistinctGreaterThan::Operation(left, right, right_null, left_null);
	}
};

struct DistinctGreaterThanEquals {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return !DistinctGreaterThan::Operation(right, left, right_null, left_null);
	}
};

struct DistinctLessThan {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return DistinctGreaterThan::Operation(right, left, right_null, left_null);
	}
};

struct DistinctLessThanNullsFirst {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return DistinctGreaterThan::Operation(right, left, left_null, right_null);
	}
};

struct DistinctLessThanEquals {
	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return !DistinctGreaterThan::Operation(left, right, left_null, right_null);
	}
};

//===--------------------------------------------------------------------===//
// Comparison Operator Wrappers (so (Not)DistinctFrom have the same API)
//===--------------------------------------------------------------------===//
template <class OP>
struct ComparisonOperationWrapper {
	static constexpr const bool COMPARE_NULL = false;

	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		if (right_null || left_null) {
			return false;
		}
		return OP::template Operation<T>(left, right);
	}
};

template <>
struct ComparisonOperationWrapper<DistinctFrom> {
	static constexpr const bool COMPARE_NULL = true;

	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return DistinctFrom::template Operation<T>(left, right, left_null, right_null);
	}
};

template <>
struct ComparisonOperationWrapper<NotDistinctFrom> {
	static constexpr const bool COMPARE_NULL = true;

	template <class T>
	static inline bool Operation(const T &left, const T &right, bool left_null, bool right_null) {
		return NotDistinctFrom::template Operation<T>(left, right, left_null, right_null);
	}
};

//===--------------------------------------------------------------------===//
// Specialized Boolean Comparison Operators
//===--------------------------------------------------------------------===//
template <>
inline bool GreaterThan::Operation(const bool &left, const bool &right) {
	return !right && left;
}
//===--------------------------------------------------------------------===//
// Specialized String Comparison Operations
//===--------------------------------------------------------------------===//
template <>
inline bool Equals::Operation(const string_t &left, const string_t &right) {
	return left == right;
}

template <>
inline bool GreaterThan::Operation(const string_t &left, const string_t &right) {
	return left > right;
}

//===--------------------------------------------------------------------===//
// Specialized Interval Comparison Operators
//===--------------------------------------------------------------------===//
template <>
inline bool Equals::Operation(const interval_t &left, const interval_t &right) {
	return Interval::Equals(left, right);
}
template <>
inline bool GreaterThan::Operation(const interval_t &left, const interval_t &right) {
	return Interval::GreaterThan(left, right);
}

} // namespace goose
