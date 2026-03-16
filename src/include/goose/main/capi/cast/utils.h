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

#include <goose/main/capi/capi_internal.h>
#include <goose/common/operator/cast_operators.h>
#include <goose/common/operator/string_cast.h>
#include <goose/common/operator/decimal_cast_operators.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Unsafe Fetch (for internal use only)
//===--------------------------------------------------------------------===//
template <class T>
T UnsafeFetchFromPtr(void *pointer) {
	return *((T *)pointer);
}

template <class T>
void *UnsafeFetchPtr(goose_result *result, idx_t col, idx_t row) {
	D_ASSERT(row < result->deprecated_row_count);
	return (void *)&(((T *)result->deprecated_columns[col].deprecated_data)[row]);
}

template <class T>
T UnsafeFetch(goose_result *result, idx_t col, idx_t row) {
	return UnsafeFetchFromPtr<T>(UnsafeFetchPtr<T>(result, col, row));
}

//===--------------------------------------------------------------------===//
// Fetch Default Value
//===--------------------------------------------------------------------===//
struct FetchDefaultValue {
	template <class T>
	static T Operation() {
		return 0;
	}
};

template <>
goose_decimal FetchDefaultValue::Operation();
template <>
date_t FetchDefaultValue::Operation();
template <>
dtime_t FetchDefaultValue::Operation();
template <>
timestamp_t FetchDefaultValue::Operation();
template <>
interval_t FetchDefaultValue::Operation();
template <>
char *FetchDefaultValue::Operation();
template <>
goose_string FetchDefaultValue::Operation();
template <>
goose_blob FetchDefaultValue::Operation();

//===--------------------------------------------------------------------===//
// String Casts
//===--------------------------------------------------------------------===//
template <class OP>
struct FromCStringCastWrapper {
	template <class SOURCE_TYPE, class RESULT_TYPE>
	static bool Operation(SOURCE_TYPE input_str, RESULT_TYPE &result) {
		string_t input(input_str);
		return OP::template Operation<string_t, RESULT_TYPE>(input, result);
	}
};

template <class OP>
struct ToCStringCastWrapper {
	template <class SOURCE_TYPE, class RESULT_TYPE>
	static bool Operation(SOURCE_TYPE input, RESULT_TYPE &result) {
		Vector result_vector(LogicalType::VARCHAR, nullptr);
		auto result_string = OP::template Operation<SOURCE_TYPE>(input, result_vector);
		auto result_size = result_string.GetSize();
		auto result_data = result_string.GetData();

		char *allocated_data = char_ptr_cast(goose_malloc(result_size + 1));
		memcpy(allocated_data, result_data, result_size);
		allocated_data[result_size] = '\0';
		result.data = allocated_data;
		result.size = result_size;
		return true;
	}
};

//===--------------------------------------------------------------------===//
// Blob Casts
//===--------------------------------------------------------------------===//
struct FromCBlobCastWrapper {
	template <class SOURCE_TYPE, class RESULT_TYPE>
	static bool Operation(SOURCE_TYPE input_str, RESULT_TYPE &result) {
		return false;
	}
};

template <>
bool FromCBlobCastWrapper::Operation(goose_blob input, goose_string &result);

template <class SOURCE_TYPE, class RESULT_TYPE, class OP>
RESULT_TYPE TryCastCInternal(goose_result *result, idx_t col, idx_t row) {
	RESULT_TYPE result_value;
	try {
		if (!OP::template Operation<SOURCE_TYPE, RESULT_TYPE>(UnsafeFetch<SOURCE_TYPE>(result, col, row),
		                                                      result_value)) {
			return FetchDefaultValue::Operation<RESULT_TYPE>();
		}
	} catch (...) {
		return FetchDefaultValue::Operation<RESULT_TYPE>();
	}
	return result_value;
}

} // namespace goose

bool CanFetchValue(goose_result *result, idx_t col, idx_t row);
bool CanUseDeprecatedFetch(goose_result *result, idx_t col, idx_t row);
