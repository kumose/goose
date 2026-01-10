#include <goose/main/capi/capi_internal.h>
#include <goose/common/types/date.h>
#include <goose/common/types/time.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/types.h>
#include <goose/common/uhugeint.h>

#include <goose/main/capi/cast/generic.h>

#include <cstring>

using goose::date_t;
using goose::dtime_t;
using goose::FetchDefaultValue;
using goose::GetInternalCValue;
using goose::hugeint_t;
using goose::interval_t;
using goose::StringCast;
using goose::timestamp_t;
using goose::ToCStringCastWrapper;
using goose::uhugeint_t;
using goose::UnsafeFetch;

bool goose_value_boolean(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<bool>(result, col, row);
}

int8_t goose_value_int8(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<int8_t>(result, col, row);
}

int16_t goose_value_int16(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<int16_t>(result, col, row);
}

int32_t goose_value_int32(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<int32_t>(result, col, row);
}

int64_t goose_value_int64(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<int64_t>(result, col, row);
}

static bool ResultIsDecimal(goose_result *result, idx_t col) {
	if (!result) {
		return false;
	}
	if (!result->internal_data) {
		return false;
	}
	auto result_data = (goose::GooseResultData *)result->internal_data;
	auto &query_result = result_data->result;
	auto &source_type = query_result->types[col];
	return source_type.id() == goose::LogicalTypeId::DECIMAL;
}

goose_decimal goose_value_decimal(goose_result *result, idx_t col, idx_t row) {
	if (!CanFetchValue(result, col, row) || !ResultIsDecimal(result, col)) {
		return FetchDefaultValue::Operation<goose_decimal>();
	}

	return GetInternalCValue<goose_decimal>(result, col, row);
}

goose_hugeint goose_value_hugeint(goose_result *result, idx_t col, idx_t row) {
	goose_hugeint result_value;
	auto internal_value = GetInternalCValue<hugeint_t>(result, col, row);
	result_value.lower = internal_value.lower;
	result_value.upper = internal_value.upper;
	return result_value;
}

goose_uhugeint goose_value_uhugeint(goose_result *result, idx_t col, idx_t row) {
	goose_uhugeint result_value;
	auto internal_value = GetInternalCValue<uhugeint_t>(result, col, row);
	result_value.lower = internal_value.lower;
	result_value.upper = internal_value.upper;
	return result_value;
}

uint8_t goose_value_uint8(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<uint8_t>(result, col, row);
}

uint16_t goose_value_uint16(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<uint16_t>(result, col, row);
}

uint32_t goose_value_uint32(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<uint32_t>(result, col, row);
}

uint64_t goose_value_uint64(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<uint64_t>(result, col, row);
}

float goose_value_float(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<float>(result, col, row);
}

double goose_value_double(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<double>(result, col, row);
}

goose_date goose_value_date(goose_result *result, idx_t col, idx_t row) {
	goose_date result_value;
	result_value.days = GetInternalCValue<date_t>(result, col, row).days;
	return result_value;
}

goose_time goose_value_time(goose_result *result, idx_t col, idx_t row) {
	goose_time result_value;
	result_value.micros = GetInternalCValue<dtime_t>(result, col, row).micros;
	return result_value;
}

goose_timestamp goose_value_timestamp(goose_result *result, idx_t col, idx_t row) {
	goose_timestamp result_value;
	result_value.micros = GetInternalCValue<timestamp_t>(result, col, row).value;
	return result_value;
}

goose_interval goose_value_interval(goose_result *result, idx_t col, idx_t row) {
	goose_interval result_value;
	auto ival = GetInternalCValue<interval_t>(result, col, row);
	result_value.months = ival.months;
	result_value.days = ival.days;
	result_value.micros = ival.micros;
	return result_value;
}

char *goose_value_varchar(goose_result *result, idx_t col, idx_t row) {
	return goose_value_string(result, col, row).data;
}

goose_string goose_value_string(goose_result *result, idx_t col, idx_t row) {
	return GetInternalCValue<goose_string, ToCStringCastWrapper<StringCast>>(result, col, row);
}

char *goose_value_varchar_internal(goose_result *result, idx_t col, idx_t row) {
	return goose_value_string_internal(result, col, row).data;
}

goose_string goose_value_string_internal(goose_result *result, idx_t col, idx_t row) {
	if (!CanFetchValue(result, col, row)) {
		return FetchDefaultValue::Operation<goose_string>();
	}
	if (goose_column_type(result, col) != GOOSE_TYPE_VARCHAR) {
		return FetchDefaultValue::Operation<goose_string>();
	}
	// FIXME: this obviously does not work when there are null bytes in the string
	// we need to remove the deprecated C result materialization to get that to work correctly
	// since the deprecated C result materialization stores strings as null-terminated
	goose_string res;
	res.data = UnsafeFetch<char *>(result, col, row);
	res.size = strlen(res.data);
	return res;
}

goose_blob goose_value_blob(goose_result *result, idx_t col, idx_t row) {
	if (CanFetchValue(result, col, row) && result->deprecated_columns[col].deprecated_type == GOOSE_TYPE_BLOB) {
		auto internal_result = UnsafeFetch<goose_blob>(result, col, row);

		goose_blob result_blob;
		result_blob.data = malloc(internal_result.size);
		result_blob.size = internal_result.size;
		memcpy(result_blob.data, internal_result.data, internal_result.size);
		return result_blob;
	}
	return FetchDefaultValue::Operation<goose_blob>();
}

bool goose_value_is_null(goose_result *result, idx_t col, idx_t row) {
	if (!CanUseDeprecatedFetch(result, col, row)) {
		return false;
	}
	return result->deprecated_columns[col].deprecated_nullmask[row];
}
