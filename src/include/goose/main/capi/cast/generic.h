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
