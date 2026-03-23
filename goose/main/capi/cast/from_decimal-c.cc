#include <goose/main/capi/cast/from_decimal.h>
#include <goose/common/types/decimal.h>

namespace goose {

//! DECIMAL -> VARCHAR
template <>
bool CastDecimalCInternal(goose_result *source, goose_string &result, idx_t col, idx_t row) {
	auto result_data = (goose::GooseResultData *)source->internal_data;
	auto &query_result = result_data->result;
	auto &source_type = query_result->types[col];
	auto width = goose::DecimalType::GetWidth(source_type);
	auto scale = goose::DecimalType::GetScale(source_type);
	goose::Vector result_vec(goose::LogicalType::VARCHAR, false, false);
	goose::string_t result_string;
	auto source_value = UnsafeFetch<hugeint_t>(source, col, row);
	switch (source_type.InternalType()) {
	case goose::PhysicalType::INT16:
		result_string = goose::StringCastFromDecimal::Operation<int16_t>(static_cast<int16_t>(source_value), width,
		                                                                  scale, result_vec);
		break;
	case goose::PhysicalType::INT32:
		result_string = goose::StringCastFromDecimal::Operation<int32_t>(static_cast<int32_t>(source_value), width,
		                                                                  scale, result_vec);
		break;
	case goose::PhysicalType::INT64:
		result_string = goose::StringCastFromDecimal::Operation<int64_t>(static_cast<int64_t>(source_value), width,
		                                                                  scale, result_vec);
		break;
	case goose::PhysicalType::INT128:
		result_string = goose::StringCastFromDecimal::Operation<hugeint_t>(source_value, width, scale, result_vec);
		break;
	default:
		throw goose::InternalException("Unimplemented internal type for decimal");
	}
	result.data = reinterpret_cast<char *>(goose_malloc(sizeof(char) * (result_string.GetSize() + 1)));
	memcpy(result.data, result_string.GetData(), result_string.GetSize());
	result.data[result_string.GetSize()] = '\0';
	result.size = result_string.GetSize();
	return true;
}

template <class INTERNAL_TYPE>
goose_hugeint FetchInternals(void *source_address) {
	throw goose::NotImplementedException("FetchInternals not implemented for internal type");
}

template <>
goose_hugeint FetchInternals<int16_t>(void *source_address) {
	const int16_t source_value = static_cast<int16_t>(UnsafeFetchFromPtr<int64_t>(source_address));
	goose_hugeint result;
	int16_t intermediate_result;

	if (!TryCast::Operation<int16_t, int16_t>(source_value, intermediate_result)) {
		intermediate_result = FetchDefaultValue::Operation<int16_t>();
	}
	hugeint_t hugeint_result = Hugeint::Cast<int16_t>(intermediate_result);
	result.lower = hugeint_result.lower;
	result.upper = hugeint_result.upper;
	return result;
}
template <>
goose_hugeint FetchInternals<int32_t>(void *source_address) {
	const int32_t source_value = static_cast<int32_t>(UnsafeFetchFromPtr<int64_t>(source_address));
	goose_hugeint result;
	int32_t intermediate_result;

	if (!TryCast::Operation<int32_t, int32_t>(source_value, intermediate_result)) {
		intermediate_result = FetchDefaultValue::Operation<int32_t>();
	}
	hugeint_t hugeint_result = Hugeint::Cast<int32_t>(intermediate_result);
	result.lower = hugeint_result.lower;
	result.upper = hugeint_result.upper;
	return result;
}
template <>
goose_hugeint FetchInternals<int64_t>(void *source_address) {
	const int64_t source_value = UnsafeFetchFromPtr<int64_t>(source_address);
	goose_hugeint result;
	int64_t intermediate_result;

	if (!TryCast::Operation<int64_t, int64_t>(source_value, intermediate_result)) {
		intermediate_result = FetchDefaultValue::Operation<int64_t>();
	}
	hugeint_t hugeint_result = Hugeint::Cast<int64_t>(intermediate_result);
	result.lower = hugeint_result.lower;
	result.upper = hugeint_result.upper;
	return result;
}
template <>
goose_hugeint FetchInternals<hugeint_t>(void *source_address) {
	const hugeint_t source_value = UnsafeFetchFromPtr<hugeint_t>(source_address);
	goose_hugeint result;
	hugeint_t intermediate_result;

	if (!TryCast::Operation<hugeint_t, hugeint_t>(source_value, intermediate_result)) {
		intermediate_result = FetchDefaultValue::Operation<hugeint_t>();
	}
	result.lower = intermediate_result.lower;
	result.upper = intermediate_result.upper;
	return result;
}

//! DECIMAL -> DECIMAL (internal fetch)
template <>
bool CastDecimalCInternal(goose_result *source, goose_decimal &result, idx_t col, idx_t row) {
	auto result_data = (goose::GooseResultData *)source->internal_data;
	result_data->result->types[col].GetDecimalProperties(result.width, result.scale);
	auto source_address = UnsafeFetchPtr<hugeint_t>(source, col, row);

	if (result.width > goose::Decimal::MAX_WIDTH_INT64) {
		result.value = FetchInternals<hugeint_t>(source_address);
	} else if (result.width > goose::Decimal::MAX_WIDTH_INT32) {
		result.value = FetchInternals<int64_t>(source_address);
	} else if (result.width > goose::Decimal::MAX_WIDTH_INT16) {
		result.value = FetchInternals<int32_t>(source_address);
	} else {
		result.value = FetchInternals<int16_t>(source_address);
	}
	return true;
}

} // namespace goose
