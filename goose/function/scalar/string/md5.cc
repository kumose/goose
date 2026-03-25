#include <goose/common/crypto/md5.h>

#include <goose/function/scalar/string_functions.h>
#include <goose/common/exception.h>
#include <goose/common/vector_operations/unary_executor.h>

namespace goose {

namespace {

struct MD5Operator {
	template <class INPUT_TYPE, class RESULT_TYPE>
	static RESULT_TYPE Operation(INPUT_TYPE input, Vector &result) {
		auto hash = StringVector::EmptyString(result, MD5Context::MD5_HASH_LENGTH_TEXT);
		MD5Context context;
		context.Add(input);
		context.FinishHex(hash.GetDataWriteable());
		hash.Finalize();
		return hash;
	}
};

struct MD5Number128Operator {
	template <class INPUT_TYPE, class RESULT_TYPE>
	static RESULT_TYPE Operation(INPUT_TYPE input) {
		data_t digest[MD5Context::MD5_HASH_LENGTH_BINARY];

		MD5Context context;
		context.Add(input);
		context.Finish(digest);
		return BSwapIfBE(*reinterpret_cast<uhugeint_t *>(digest));
	}
};

void MD5Function(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &input = args.data[0];

	UnaryExecutor::ExecuteString<string_t, string_t, MD5Operator>(input, result, args.size());
}

void MD5NumberFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &input = args.data[0];

	UnaryExecutor::Execute<string_t, uhugeint_t, MD5Number128Operator>(input, result, args.size());
}

} // namespace

ScalarFunctionSet MD5Fun::GetFunctions() {
	ScalarFunctionSet set("md5");
	set.AddFunction(ScalarFunction({LogicalType::VARCHAR}, LogicalType::VARCHAR, MD5Function));
	set.AddFunction(ScalarFunction({LogicalType::BLOB}, LogicalType::VARCHAR, MD5Function));
	return set;
}

ScalarFunctionSet MD5NumberFun::GetFunctions() {
	ScalarFunctionSet set("md5_number");
	set.AddFunction(ScalarFunction({LogicalType::VARCHAR}, LogicalType::UHUGEINT, MD5NumberFunction));
	set.AddFunction(ScalarFunction({LogicalType::BLOB}, LogicalType::UHUGEINT, MD5NumberFunction));
	return set;
}

} // namespace goose
