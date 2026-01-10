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
