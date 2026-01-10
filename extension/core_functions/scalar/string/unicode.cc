#include "core_functions/scalar/string_functions.h"

#include <goose/common/exception.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/common/vector_operations/unary_executor.h>
#include <goose/utility/utf8proc.h>

#include <string.h>

namespace goose {

struct UnicodeOperator {
	template <class TA, class TR>
	static inline TR Operation(const TA &input) {
		auto str = reinterpret_cast<const utf8proc_uint8_t *>(input.GetData());
		auto len = input.GetSize();
		utf8proc_int32_t codepoint;
		(void)utf8proc_iterate(str, UnsafeNumericCast<utf8proc_ssize_t>(len), &codepoint);
		return codepoint;
	}
};

ScalarFunction UnicodeFun::GetFunction() {
	return ScalarFunction({LogicalType::VARCHAR}, LogicalType::INTEGER,
	                      ScalarFunction::UnaryFunction<string_t, int32_t, UnicodeOperator>);
}

} // namespace goose
