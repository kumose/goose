#include <goose/function/scalar/string_functions.h>
#include <goose/common/exception.h>
#include <goose/common/vector_operations/unary_executor.h>
#include <goose/utility/mbedtls_wrapper.h>

namespace goose {

namespace {

struct SHA256Operator {
	template <class INPUT_TYPE, class RESULT_TYPE>
	static RESULT_TYPE Operation(INPUT_TYPE input, Vector &result) {
		auto hash = StringVector::EmptyString(result, goose_mbedtls::MbedTlsWrapper::SHA256_HASH_LENGTH_TEXT);

		goose_mbedtls::MbedTlsWrapper::SHA256State state;
		state.AddString(input.GetString());
		state.FinishHex(hash.GetDataWriteable());

		hash.Finalize();
		return hash;
	}
};

void SHA256Function(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &input = args.data[0];

	UnaryExecutor::ExecuteString<string_t, string_t, SHA256Operator>(input, result, args.size());
}

} // namespace

ScalarFunctionSet SHA256Fun::GetFunctions() {
	ScalarFunctionSet set("sha256");
	set.AddFunction(ScalarFunction({LogicalType::VARCHAR}, LogicalType::VARCHAR, SHA256Function));
	set.AddFunction(ScalarFunction({LogicalType::BLOB}, LogicalType::VARCHAR, SHA256Function));
	return set;
}

} // namespace goose
