#include <goose/main/capi/cast/utils.h>

namespace goose {

template <>
goose_decimal FetchDefaultValue::Operation() {
	goose_decimal result;
	result.scale = 0;
	result.width = 0;
	result.value = {0, 0};
	return result;
}

template <>
date_t FetchDefaultValue::Operation() {
	date_t result;
	result.days = 0;
	return result;
}

template <>
dtime_t FetchDefaultValue::Operation() {
	dtime_t result;
	result.micros = 0;
	return result;
}

template <>
timestamp_t FetchDefaultValue::Operation() {
	timestamp_t result;
	result.value = 0;
	return result;
}

template <>
interval_t FetchDefaultValue::Operation() {
	interval_t result;
	result.months = 0;
	result.days = 0;
	result.micros = 0;
	return result;
}

template <>
char *FetchDefaultValue::Operation() {
	return nullptr;
}

template <>
goose_string FetchDefaultValue::Operation() {
	goose_string result;
	result.data = nullptr;
	result.size = 0;
	return result;
}

template <>
goose_blob FetchDefaultValue::Operation() {
	goose_blob result;
	result.data = nullptr;
	result.size = 0;
	return result;
}

//===--------------------------------------------------------------------===//
// Blob Casts
//===--------------------------------------------------------------------===//

template <>
bool FromCBlobCastWrapper::Operation(goose_blob input, goose_string &result) {
	string_t input_str(const_char_ptr_cast(input.data), UnsafeNumericCast<uint32_t>(input.size));
	return ToCStringCastWrapper<goose::CastFromBlob>::template Operation<string_t, goose_string>(input_str, result);
}

} // namespace goose

bool CanUseDeprecatedFetch(goose_result *result, idx_t col, idx_t row) {
	if (!result) {
		return false;
	}
	if (!goose::DeprecatedMaterializeResult(result)) {
		return false;
	}
	if (col >= result->deprecated_column_count || row >= result->deprecated_row_count) {
		return false;
	}
	return true;
}

bool CanFetchValue(goose_result *result, idx_t col, idx_t row) {
	if (!CanUseDeprecatedFetch(result, col, row)) {
		return false;
	}
	if (result->deprecated_columns[col].deprecated_nullmask[row]) {
		return false;
	}
	return true;
}
