#include <goose/main/capi/capi_internal.h>
#include <goose/common/types/hugeint.h>
#include <goose/common/types/uhugeint.h>
#include <goose/common/types/decimal.h>
#include <goose/common/operator/decimal_cast_operators.h>
#include <goose/main/capi/cast/utils.h>
#include <goose/main/capi/cast/to_decimal.h>

using goose::Hugeint;
using goose::hugeint_t;
using goose::Uhugeint;
using goose::uhugeint_t;
using goose::Value;

double goose_hugeint_to_double(goose_hugeint val) {
	hugeint_t internal;
	internal.lower = val.lower;
	internal.upper = val.upper;
	return Hugeint::Cast<double>(internal);
}

double goose_uhugeint_to_double(goose_uhugeint val) {
	uhugeint_t internal;
	internal.lower = val.lower;
	internal.upper = val.upper;
	return Uhugeint::Cast<double>(internal);
}

static goose_decimal to_decimal_cast(double val, uint8_t width, uint8_t scale) {
	if (width > goose::Decimal::MAX_WIDTH_INT64) {
		return goose::TryCastToDecimalCInternal<double, goose::ToCDecimalCastWrapper<hugeint_t>>(val, width, scale);
	}
	if (width > goose::Decimal::MAX_WIDTH_INT32) {
		return goose::TryCastToDecimalCInternal<double, goose::ToCDecimalCastWrapper<int64_t>>(val, width, scale);
	}
	if (width > goose::Decimal::MAX_WIDTH_INT16) {
		return goose::TryCastToDecimalCInternal<double, goose::ToCDecimalCastWrapper<int32_t>>(val, width, scale);
	}
	return goose::TryCastToDecimalCInternal<double, goose::ToCDecimalCastWrapper<int16_t>>(val, width, scale);
}

goose_decimal goose_double_to_decimal(double val, uint8_t width, uint8_t scale) {
	if (scale > width || width > goose::Decimal::MAX_WIDTH_INT128) {
		return goose::FetchDefaultValue::Operation<goose_decimal>();
	}
	return to_decimal_cast(val, width, scale);
}

goose_hugeint goose_double_to_hugeint(double val) {
	hugeint_t internal_result;
	if (!Value::DoubleIsFinite(val) || !Hugeint::TryConvert<double>(val, internal_result)) {
		internal_result.lower = 0;
		internal_result.upper = 0;
	}

	goose_hugeint result;
	result.lower = internal_result.lower;
	result.upper = internal_result.upper;
	return result;
}

goose_uhugeint goose_double_to_uhugeint(double val) {
	uhugeint_t internal_result;
	if (!Value::DoubleIsFinite(val) || !Uhugeint::TryConvert<double>(val, internal_result)) {
		internal_result.lower = 0;
		internal_result.upper = 0;
	}

	goose_uhugeint result;
	result.lower = internal_result.lower;
	result.upper = internal_result.upper;
	return result;
}

double goose_decimal_to_double(goose_decimal val) {
	double result;
	hugeint_t value;
	value.lower = val.value.lower;
	value.upper = val.value.upper;
	goose::CastParameters parameters;
	goose::TryCastFromDecimal::Operation<hugeint_t, double>(value, result, parameters, val.width, val.scale);
	return result;
}
