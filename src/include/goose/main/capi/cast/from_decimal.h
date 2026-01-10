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

//! DECIMAL -> ?
template <class RESULT_TYPE>
bool CastDecimalCInternal(goose_result *source, RESULT_TYPE &result, idx_t col, idx_t row) {
	auto result_data = (goose::GooseResultData *)source->internal_data;
	auto &query_result = result_data->result;
	auto &source_type = query_result->types[col];
	auto width = goose::DecimalType::GetWidth(source_type);
	auto scale = goose::DecimalType::GetScale(source_type);
	auto source_value = UnsafeFetch<hugeint_t>(source, col, row);
	CastParameters parameters;
	switch (source_type.InternalType()) {
	case goose::PhysicalType::INT16:
		return goose::TryCastFromDecimal::Operation<int16_t, RESULT_TYPE>(static_cast<int16_t>(source_value), result,
		                                                                   parameters, width, scale);
	case goose::PhysicalType::INT32:
		return goose::TryCastFromDecimal::Operation<int32_t, RESULT_TYPE>(static_cast<int32_t>(source_value), result,
		                                                                   parameters, width, scale);
	case goose::PhysicalType::INT64:
		return goose::TryCastFromDecimal::Operation<int64_t, RESULT_TYPE>(static_cast<int64_t>(source_value), result,
		                                                                   parameters, width, scale);
	case goose::PhysicalType::INT128:
		return goose::TryCastFromDecimal::Operation<hugeint_t, RESULT_TYPE>(source_value, result, parameters, width,
		                                                                     scale);
	default:
		throw goose::InternalException("Unimplemented internal type for decimal");
	}
}

//! DECIMAL -> VARCHAR
template <>
bool CastDecimalCInternal(goose_result *source, goose_string &result, idx_t col, idx_t row);

//! DECIMAL -> DECIMAL (internal fetch)
template <>
bool CastDecimalCInternal(goose_result *source, goose_decimal &result, idx_t col, idx_t row);

//! DECIMAL -> ...
template <class RESULT_TYPE>
RESULT_TYPE TryCastDecimalCInternal(goose_result *source, idx_t col, idx_t row) {
	RESULT_TYPE result_value;
	try {
		if (!CastDecimalCInternal<RESULT_TYPE>(source, result_value, col, row)) {
			return FetchDefaultValue::Operation<RESULT_TYPE>();
		}
	} catch (...) {
		return FetchDefaultValue::Operation<RESULT_TYPE>();
	}
	return result_value;
}

} // namespace goose
