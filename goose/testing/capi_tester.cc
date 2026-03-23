#include "capi_tester.h"

bool NO_FAIL(goose::CAPIResult &result) {
	if (result.HasError()) {
		fprintf(stderr, "Query failed with message: %s\n", result.ErrorMessage());
	}
	return result.success;
}

bool NO_FAIL(goose::unique_ptr<goose::CAPIResult> result) {
	return NO_FAIL(*result);
}

namespace goose {

template <>
bool CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_boolean(&result, col, row);
}

template <>
int8_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_int8(&result, col, row);
}

template <>
int16_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_int16(&result, col, row);
}

template <>
int32_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_int32(&result, col, row);
}

template <>
int64_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_int64(&result, col, row);
}

template <>
uint8_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_uint8(&result, col, row);
}

template <>
uint16_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_uint16(&result, col, row);
}

template <>
uint32_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_uint32(&result, col, row);
}

template <>
uint64_t CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_uint64(&result, col, row);
}

template <>
float CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_float(&result, col, row);
}

template <>
double CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_double(&result, col, row);
}

template <>
goose_decimal CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_decimal(&result, col, row);
}

template <>
goose_date CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_date *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_time CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_time *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_time_ns CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_time_ns *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_timestamp CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_timestamp *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_timestamp_s CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_timestamp_s *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_timestamp_ms CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_timestamp_ms *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_timestamp_ns CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_timestamp_ns *)goose_column_data(&result, col);
	return data[row];
}

template <>
goose_interval CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_interval(&result, col, row);
}

template <>
goose_blob CAPIResult::Fetch(idx_t col, idx_t row) {
	auto data = (goose_blob *)goose_column_data(&result, col);
	return data[row];
}

template <>
string CAPIResult::Fetch(idx_t col, idx_t row) {
	auto value = goose_value_varchar(&result, col, row);
	string strval = value ? string(value) : string();
	free((void *)value);
	return strval;
}

template <>
goose_date_struct CAPIResult::Fetch(idx_t col, idx_t row) {
	auto value = goose_value_date(&result, col, row);
	return goose_from_date(value);
}

template <>
goose_time_struct CAPIResult::Fetch(idx_t col, idx_t row) {
	auto value = goose_value_time(&result, col, row);
	return goose_from_time(value);
}

template <>
goose_timestamp_struct CAPIResult::Fetch(idx_t col, idx_t row) {
	auto value = goose_value_timestamp(&result, col, row);
	return goose_from_timestamp(value);
}

template <>
goose_hugeint CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_hugeint(&result, col, row);
}

template <>
goose_uhugeint CAPIResult::Fetch(idx_t col, idx_t row) {
	return goose_value_uhugeint(&result, col, row);
}

} // namespace goose
