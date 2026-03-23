#include <goose/common/exception.h>
#include <goose/common/types.h>
#include <goose/common/helper.h> // defines GOOSE_EXPLICIT_FALLTHROUGH which fmt will use to annotate
#include <goose/utility/fmt/format.h>
#include <goose/utility/fmt/printf.h>
#include <goose/common/types/hugeint.h>
#include <goose/common/types/uhugeint.h>
#include <goose/parser/keyword_helper.h>
#include <goose/common/types/string.h>

namespace goose {

ExceptionFormatValue::ExceptionFormatValue(double dbl_val)
    : type(ExceptionFormatValueType::FORMAT_VALUE_TYPE_DOUBLE), dbl_val(dbl_val) {
}
ExceptionFormatValue::ExceptionFormatValue(int64_t int_val)
    : type(ExceptionFormatValueType::FORMAT_VALUE_TYPE_INTEGER), int_val(int_val) {
}
ExceptionFormatValue::ExceptionFormatValue(idx_t uint_val)
    : type(ExceptionFormatValueType::FORMAT_VALUE_TYPE_INTEGER), int_val(Hugeint::Convert(uint_val)) {
}
ExceptionFormatValue::ExceptionFormatValue(hugeint_t huge_val)
    : type(ExceptionFormatValueType::FORMAT_VALUE_TYPE_STRING), str_val(Hugeint::ToString(huge_val)) {
}
ExceptionFormatValue::ExceptionFormatValue(uhugeint_t uhuge_val)
    : type(ExceptionFormatValueType::FORMAT_VALUE_TYPE_STRING), str_val(Uhugeint::ToString(uhuge_val)) {
}
ExceptionFormatValue::ExceptionFormatValue(string str_val)
    : type(ExceptionFormatValueType::FORMAT_VALUE_TYPE_STRING), str_val(std::move(str_val)) {
}
ExceptionFormatValue::ExceptionFormatValue(const String &str_val) : ExceptionFormatValue(str_val.ToStdString()) {
}

template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const PhysicalType &value) {
	return ExceptionFormatValue(TypeIdToString(value));
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const LogicalType &value) {
	return ExceptionFormatValue(value.ToString());
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const float &value) {
	return ExceptionFormatValue(static_cast<double>(value));
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const double &value) {
	return ExceptionFormatValue(value);
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const string &value) {
	return ExceptionFormatValue(value);
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const String &value) {
	return ExceptionFormatValue(value);
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const SQLString &value) {
	return KeywordHelper::WriteQuoted(value.raw_string, '\'');
}

template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const SQLIdentifier &value) {
	return KeywordHelper::WriteOptionallyQuoted(value.raw_string, '"');
}

template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const char *const &value) {
	return ExceptionFormatValue(string(value));
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(char *const &value) {
	return ExceptionFormatValue(string(value));
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const idx_t &value) {
	return ExceptionFormatValue(value);
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const hugeint_t &value) {
	return ExceptionFormatValue(value);
}
template <>
ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const uhugeint_t &value) {
	return ExceptionFormatValue(value);
}

string ExceptionFormatValue::Format(const string &msg, std::vector<ExceptionFormatValue> &values) {
	try {
		std::vector<goosedb_fmt::basic_format_arg<goosedb_fmt::printf_context>> format_args;
		for (auto &val : values) {
			switch (val.type) {
			case ExceptionFormatValueType::FORMAT_VALUE_TYPE_DOUBLE:
				format_args.push_back(goosedb_fmt::internal::make_arg<goosedb_fmt::printf_context>(val.dbl_val));
				break;
			case ExceptionFormatValueType::FORMAT_VALUE_TYPE_INTEGER:
				format_args.push_back(goosedb_fmt::internal::make_arg<goosedb_fmt::printf_context>(val.int_val));
				break;
			case ExceptionFormatValueType::FORMAT_VALUE_TYPE_STRING:
				format_args.push_back(goosedb_fmt::internal::make_arg<goosedb_fmt::printf_context>(val.str_val));
				break;
			}
		}
		return goosedb_fmt::vsprintf(msg, goosedb_fmt::basic_format_args<goosedb_fmt::printf_context>(
		                                     format_args.data(), static_cast<int>(format_args.size())));
	} catch (std::exception &ex) { // LCOV_EXCL_START
		// work-around for oss-fuzz limiting memory which causes issues here
		if (StringUtil::Contains(ex.what(), "fuzz mode")) {
			throw InvalidInputException(msg);
		}
		throw InternalException(std::string("Primary exception: ") + msg +
		                        "\nSecondary exception in ExceptionFormatValue: " + ex.what());
	} // LCOV_EXCL_STOP
}

} // namespace goose
