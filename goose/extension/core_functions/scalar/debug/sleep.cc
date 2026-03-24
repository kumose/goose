#include <goose/extension/core_functions/scalar/debug_functions.h>

#include <goose/common/vector_operations/generic_executor.h>
#include <goose/planner/expression/bound_function_expression.h>

#include <goose/common/thread.h>

namespace goose {

struct NullResultType {
	using STRUCT_STATE = PrimitiveTypeState;

	static void AssignResult(Vector &result, idx_t i, NullResultType) {
		FlatVector::SetNull(result, i, true);
	}
};

static void SleepFunction(DataChunk &input, ExpressionState &state, Vector &result) {
	input.Flatten();
	GenericExecutor::ExecuteUnary<PrimitiveType<int64_t>, NullResultType>(input.data[0], result, input.size(),
	                                                                      [](PrimitiveType<int64_t> input) {
		                                                                      // Sleep for the specified number of
		                                                                      // milliseconds (clamp negative values to
		                                                                      // 0)
		                                                                      int64_t sleep_ms = input.val;
		                                                                      if (sleep_ms < 0) {
			                                                                      sleep_ms = 0;
		                                                                      }
		                                                                      ThreadUtil::SleepMs(sleep_ms);
		                                                                      return NullResultType();
	                                                                      });
}

ScalarFunction SleepMsFun::GetFunction() {
	auto sleep_fun = ScalarFunction({LogicalType::BIGINT}, LogicalType::SQLNULL, SleepFunction, nullptr);
	sleep_fun.stability = FunctionStability::VOLATILE;
	sleep_fun.SetNullHandling(FunctionNullHandling::SPECIAL_HANDLING);
	return sleep_fun;
}

} // namespace goose
