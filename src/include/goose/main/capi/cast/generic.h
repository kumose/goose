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

#include <goose/common/types/time.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/types/date.h>

#include <goose/main/capi/capi_internal.h>
#include <goose/main/capi/cast/utils.h>
#include <goose/main/capi/cast/from_decimal.h>

namespace goose {

template <class RESULT_TYPE, class OP = goose::TryCast>
RESULT_TYPE GetInternalCValue(goose_result *result, idx_t col, idx_t row) {
	if (!CanFetchValue(result, col, row)) {
		return FetchDefaultValue::Operation<RESULT_TYPE>();
	}
	switch (result->deprecated_columns[col].deprecated_type) {
	case GOOSE_TYPE_BOOLEAN:
		return TryCastCInternal<bool, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_TINYINT:
		return TryCastCInternal<int8_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_SMALLINT:
		return TryCastCInternal<int16_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_INTEGER:
		return TryCastCInternal<int32_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_BIGINT:
		return TryCastCInternal<int64_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_UTINYINT:
		return TryCastCInternal<uint8_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_USMALLINT:
		return TryCastCInternal<uint16_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_UINTEGER:
		return TryCastCInternal<uint32_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_UBIGINT:
		return TryCastCInternal<uint64_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_FLOAT:
		return TryCastCInternal<float, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_DOUBLE:
		return TryCastCInternal<double, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_DATE:
		return TryCastCInternal<date_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_TIME:
		return TryCastCInternal<dtime_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_TIMESTAMP:
		return TryCastCInternal<timestamp_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_HUGEINT:
		return TryCastCInternal<hugeint_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_UHUGEINT:
		return TryCastCInternal<uhugeint_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_DECIMAL:
		return TryCastDecimalCInternal<RESULT_TYPE>(result, col, row);
	case GOOSE_TYPE_INTERVAL:
		return TryCastCInternal<interval_t, RESULT_TYPE, OP>(result, col, row);
	case GOOSE_TYPE_VARCHAR:
		return TryCastCInternal<char *, RESULT_TYPE, FromCStringCastWrapper<OP>>(result, col, row);
	case GOOSE_TYPE_BLOB:
		return TryCastCInternal<goose_blob, RESULT_TYPE, FromCBlobCastWrapper>(result, col, row);
	default: { // LCOV_EXCL_START
		// Invalid type for C to C++ conversion. Internally, we set the null mask to NULL.
		// This is a deprecated code path. Use the Vector Interface for nested and complex types.
		return FetchDefaultValue::Operation<RESULT_TYPE>();
	} // LCOV_EXCL_STOP
	}
}

} // namespace goose
