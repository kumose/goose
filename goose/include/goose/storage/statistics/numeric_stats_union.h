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
