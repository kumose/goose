#include <goose/extension/core_functions/scalar/string_functions.h>

#include <goose/common/exception.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/common/vector_operations/unary_executor.h>
#include <goose/utility/utf8proc_wrapper.h>

#include <string.h>

namespace goose {

//! Fast ASCII string reverse, returns false if the input data is not ascii
static bool StrReverseASCII(const char *input, idx_t n, char *output) {
	for (idx_t i = 0; i < n; i++) {
		if (input[i] & 0x80) {
			// non-ascii character
			return false;
		}
		output[n - i - 1] = input[i];
	}
	return true;
}

//! Unicode string reverse using grapheme breakers
static void StrReverseUnicode(const char *input, idx_t n, char *output) {
	for (auto cluster : Utf8Proc::GraphemeClusters(input, n)) {
		memcpy(output + n - cluster.end, input + cluster.start, cluster.end - cluster.start);
	}
}

struct ReverseOperator {
	template <class INPUT_TYPE, class RESULT_TYPE>
	static RESULT_TYPE Operation(INPUT_TYPE input, Vector &result) {
		auto input_data = input.GetData();
		auto input_length = input.GetSize();

		auto target = StringVector::EmptyString(result, input_length);
		auto target_data = target.GetDataWriteable();
		if (!StrReverseASCII(input_data, input_length, target_data)) {
			StrReverseUnicode(input_data, input_length, target_data);
		}
		target.Finalize();
		return target;
	}
};

static void ReverseFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	UnaryExecutor::ExecuteString<string_t, string_t, ReverseOperator>(args.data[0], result, args.size());
}

ScalarFunction ReverseFun::GetFunction() {
	return ScalarFunction("reverse", {LogicalType::VARCHAR}, LogicalType::VARCHAR, ReverseFunction);
}

} // namespace goose
