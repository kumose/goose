#include <goose/common/exception/conversion_exception.h>
#include <goose/common/types.h>

namespace goose {

ConversionException::ConversionException(const PhysicalType orig_type, const PhysicalType new_type)
    : Exception(ExceptionType::CONVERSION,
                "Type " + TypeIdToString(orig_type) + " can't be cast as " + TypeIdToString(new_type)) {
}

ConversionException::ConversionException(const LogicalType &orig_type, const LogicalType &new_type)
    : Exception(ExceptionType::CONVERSION,
                "Type " + orig_type.ToString() + " can't be cast as " + new_type.ToString()) {
}

ConversionException::ConversionException(const string &msg) : Exception(ExceptionType::CONVERSION, msg) {
}

ConversionException::ConversionException(optional_idx error_location, const string &msg)
    : Exception(Exception::InitializeExtraInfo(error_location), ExceptionType::CONVERSION, msg) {
}

} // namespace goose
