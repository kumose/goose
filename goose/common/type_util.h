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

#include <goose/common/types.h>
#include <goose/common/types/date.h>
#include <goose/common/types/datetime.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/types/interval.h>
#include <goose/common/uhugeint.h>
#include <goose/common/types/double_na_equal.h>

namespace goose {
struct bignum_t;

//! Returns the PhysicalType for the given type
template <class T>
PhysicalType GetTypeId() {
	using TYPE = typename std::remove_cv<T>::type;

	if (std::is_same<TYPE, bool>()) {
		return PhysicalType::BOOL;
	} else if (std::is_same<TYPE, int8_t>()) {
		return PhysicalType::INT8;
	} else if (std::is_same<TYPE, int16_t>()) {
		return PhysicalType::INT16;
	} else if (std::is_same<TYPE, int32_t>()) {
		return PhysicalType::INT32;
	} else if (std::is_same<TYPE, int64_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, uint8_t>()) {
		return PhysicalType::UINT8;
	} else if (std::is_same<TYPE, uint16_t>()) {
		return PhysicalType::UINT16;
	} else if (std::is_same<TYPE, uint32_t>()) {
		return PhysicalType::UINT32;
	} else if (std::is_same<TYPE, uint64_t>()) {
		return PhysicalType::UINT64;
	} else if (std::is_same<TYPE, idx_t>() || std::is_same<TYPE, const idx_t>()) {
		return PhysicalType::UINT64;
	} else if (std::is_same<TYPE, hugeint_t>()) {
		return PhysicalType::INT128;
	} else if (std::is_same<TYPE, uhugeint_t>()) {
		return PhysicalType::UINT128;
	} else if (std::is_same<TYPE, date_t>()) {
		return PhysicalType::INT32;
	} else if (std::is_same<TYPE, dtime_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, dtime_tz_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, dtime_ns_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, timestamp_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, timestamp_sec_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, timestamp_ms_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, timestamp_ns_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, timestamp_tz_t>()) {
		return PhysicalType::INT64;
	} else if (std::is_same<TYPE, float>() || std::is_same<TYPE, float_na_equal>()) {
		return PhysicalType::FLOAT;
	} else if (std::is_same<TYPE, double>() || std::is_same<TYPE, double_na_equal>()) {
		return PhysicalType::DOUBLE;
	} else if (std::is_same<TYPE, const char *>() || std::is_same<TYPE, char *>() || std::is_same<TYPE, string_t>() ||
	           std::is_same<TYPE, bignum_t>()) {
		return PhysicalType::VARCHAR;
	} else if (std::is_same<TYPE, interval_t>()) {
		return PhysicalType::INTERVAL;
	} else if (std::is_same<TYPE, list_entry_t>()) {
		return PhysicalType::LIST;
	} else if (std::is_pointer<TYPE>() || std::is_same<TYPE, uintptr_t>()) {
		if (sizeof(uintptr_t) == sizeof(uint32_t)) {
			return PhysicalType::UINT32;
		} else if (sizeof(uintptr_t) == sizeof(uint64_t)) {
			return PhysicalType::UINT64;
		} else {
			throw InternalException("Unsupported pointer size in GetTypeId");
		}
	} else {
		throw InternalException("Unsupported type in GetTypeId");
	}
}

template <class T>
bool StorageTypeCompatible(PhysicalType type) {
	using TYPE = typename std::remove_cv<T>::type;

	if (std::is_same<TYPE, int8_t>()) {
		return type == PhysicalType::INT8 || type == PhysicalType::BOOL;
	}
	if (std::is_same<TYPE, uint8_t>()) {
		return type == PhysicalType::UINT8 || type == PhysicalType::BOOL;
	}
	return type == GetTypeId<T>();
}

template <class T>
bool TypeIsNumber() {
	using TYPE = typename std::remove_cv<T>::type;

	return std::is_integral<TYPE>() || std::is_floating_point<TYPE>() || std::is_same<TYPE, hugeint_t>() ||
	       std::is_same<TYPE, uhugeint_t>();
}

template <class T>
bool IsValidType() {
	return GetTypeId<T>() != PhysicalType::INVALID;
}

template <class T>
bool IsIntegerType() {
	return TypeIsIntegral(GetTypeId<T>());
}

} // namespace goose
