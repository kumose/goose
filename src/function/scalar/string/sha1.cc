#include <goose/function/scalar/string_functions.h>
#include <goose/common/exception.h>
#include <goose/common/vector_operations/unary_executor.h>
#include <goose/utility/mbedtls_wrapper.h>

namespace goose {

namespace {

struct SHA1Operator {
	template <class INPUT_TYPE, class RESULT_TYPE>
	static RESULT_TYPE Operation(INPUT_TYPE input, Vector &result) {
		auto hash = StringVector::EmptyString(result, goose_mbedtls::MbedTlsWrapper::SHA1_HASH_LENGTH_TEXT);

		goose_mbedtls::MbedTlsWrapper::SHA1State state;
		state.AddString(input.GetString());
		state.FinishHex(hash.GetDataWriteable());

		hash.Finalize();
		return hash;
	}
};

void SHA1Function(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &input = args.data[0];

	UnaryExecutor::ExecuteString<string_t, string_t, SHA1Operator>(input, result, args.size());
}

} // namespace

ScalarFunctionSet SHA1Fun::GetFunctions() {
	ScalarFunctionSet set("sha1");
	set.AddFunction(ScalarFunction({LogicalType::VARCHAR}, LogicalType::VARCHAR, SHA1Function));
	set.AddFunction(ScalarFunction({LogicalType::BLOB}, LogicalType::VARCHAR, SHA1Function));
	return set;
}

} // namespace goose
