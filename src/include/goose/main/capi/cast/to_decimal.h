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

#include <goose/main/capi/cast/utils.h>

namespace goose {

template <class INTERNAL_TYPE>
struct ToCDecimalCastWrapper {
	template <class SOURCE_TYPE>
	static bool Operation(SOURCE_TYPE input, goose_decimal &result, CastParameters &parameters, uint8_t width,
	                      uint8_t scale) {
		throw NotImplementedException("Type not implemented for CDecimalCastWrapper");
	}
};

//! Hugeint
template <>
struct ToCDecimalCastWrapper<hugeint_t> {
	template <class SOURCE_TYPE>
	static bool Operation(SOURCE_TYPE input, goose_decimal &result, CastParameters &parameters, uint8_t width,
	                      uint8_t scale) {
		hugeint_t intermediate_result;

		if (!TryCastToDecimal::Operation<SOURCE_TYPE, hugeint_t>(input, intermediate_result, parameters, width,
		                                                         scale)) {
			result = FetchDefaultValue::Operation<goose_decimal>();
			return false;
		}
		result.scale = scale;
		result.width = width;

		goose_hugeint hugeint_value;
		hugeint_value.upper = intermediate_result.upper;
		hugeint_value.lower = intermediate_result.lower;
		result.value = hugeint_value;
		return true;
	}
};

//! FIXME: reduce duplication here by just matching on the signed-ness of the type
//! INTERNAL_TYPE = int16_t
template <>
struct ToCDecimalCastWrapper<int16_t> {
	template <class SOURCE_TYPE>
	static bool Operation(SOURCE_TYPE input, goose_decimal &result, CastParameters &parameters, uint8_t width,
	                      uint8_t scale) {
		int16_t intermediate_result;

		if (!TryCastToDecimal::Operation<SOURCE_TYPE, int16_t>(input, intermediate_result, parameters, width, scale)) {
			result = FetchDefaultValue::Operation<goose_decimal>();
			return false;
		}
		hugeint_t hugeint_result = Hugeint::Convert(intermediate_result);

		result.scale = scale;
		result.width = width;

		goose_hugeint hugeint_value;
		hugeint_value.upper = hugeint_result.upper;
		hugeint_value.lower = hugeint_result.lower;
		result.value = hugeint_value;
		return true;
	}
};
//! INTERNAL_TYPE = int32_t
template <>
struct ToCDecimalCastWrapper<int32_t> {
	template <class SOURCE_TYPE>
	static bool Operation(SOURCE_TYPE input, goose_decimal &result, CastParameters &parameters, uint8_t width,
	                      uint8_t scale) {
		int32_t intermediate_result;

		if (!TryCastToDecimal::Operation<SOURCE_TYPE, int32_t>(input, intermediate_result, parameters, width, scale)) {
			result = FetchDefaultValue::Operation<goose_decimal>();
			return false;
		}
		hugeint_t hugeint_result = Hugeint::Convert(intermediate_result);

		result.scale = scale;
		result.width = width;

		goose_hugeint hugeint_value;
		hugeint_value.upper = hugeint_result.upper;
		hugeint_value.lower = hugeint_result.lower;
		result.value = hugeint_value;
		return true;
	}
};
//! INTERNAL_TYPE = int64_t
template <>
struct ToCDecimalCastWrapper<int64_t> {
	template <class SOURCE_TYPE>
	static bool Operation(SOURCE_TYPE input, goose_decimal &result, CastParameters &parameters, uint8_t width,
	                      uint8_t scale) {
		int64_t intermediate_result;

		if (!TryCastToDecimal::Operation<SOURCE_TYPE, int64_t>(input, intermediate_result, parameters, width, scale)) {
			result = FetchDefaultValue::Operation<goose_decimal>();
			return false;
		}
		hugeint_t hugeint_result = Hugeint::Convert(intermediate_result);

		result.scale = scale;
		result.width = width;

		goose_hugeint hugeint_value;
		hugeint_value.upper = hugeint_result.upper;
		hugeint_value.lower = hugeint_result.lower;
		result.value = hugeint_value;
		return true;
	}
};

template <class SOURCE_TYPE, class OP>
goose_decimal TryCastToDecimalCInternal(SOURCE_TYPE source, uint8_t width, uint8_t scale) {
	goose_decimal result;
	try {
		CastParameters parameters;
		if (!OP::template Operation<SOURCE_TYPE>(source, result, parameters, width, scale)) {
			return FetchDefaultValue::Operation<goose_decimal>();
		}
	} catch (...) {
		return FetchDefaultValue::Operation<goose_decimal>();
	}
	return result;
}

template <class SOURCE_TYPE, class OP>
goose_decimal TryCastToDecimalCInternal(goose_result *result, idx_t col, idx_t row, uint8_t width, uint8_t scale) {
	return TryCastToDecimalCInternal<SOURCE_TYPE, OP>(UnsafeFetch<SOURCE_TYPE>(result, col, row), width, scale);
}

} // namespace goose
