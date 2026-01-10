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

#include <goose/common/common.h>
#include <goose/common/exception.h>
#include <goose/common/types/hugeint.h>

namespace goose {

struct NumericValueUnion {
	union Val {
		bool boolean;
		int8_t tinyint;
		int16_t smallint;
		int32_t integer;
		int64_t bigint;
		uint8_t utinyint;
		uint16_t usmallint;
		uint32_t uinteger;
		uint64_t ubigint;
		hugeint_t hugeint;
		uhugeint_t uhugeint;
		float float_;   // NOLINT
		double double_; // NOLINT
	} value_;           // NOLINT

	template <class T>
	T &GetReferenceUnsafe();
};

template <>
GOOSE_API inline bool &NumericValueUnion::GetReferenceUnsafe() {
	return value_.boolean;
}

template <>
GOOSE_API inline int8_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.tinyint;
}

template <>
GOOSE_API inline int16_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.smallint;
}

template <>
GOOSE_API inline int32_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.integer;
}

template <>
GOOSE_API inline int64_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.bigint;
}

template <>
GOOSE_API inline hugeint_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.hugeint;
}

template <>
GOOSE_API inline uhugeint_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.uhugeint;
}

template <>
GOOSE_API inline uint8_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.utinyint;
}

template <>
GOOSE_API inline uint16_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.usmallint;
}

template <>
GOOSE_API inline uint32_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.uinteger;
}

template <>
GOOSE_API inline uint64_t &NumericValueUnion::GetReferenceUnsafe() {
	return value_.ubigint;
}

template <>
GOOSE_API inline float &NumericValueUnion::GetReferenceUnsafe() {
	return value_.float_;
}

template <>
GOOSE_API inline double &NumericValueUnion::GetReferenceUnsafe() {
	return value_.double_;
}

} // namespace goose
