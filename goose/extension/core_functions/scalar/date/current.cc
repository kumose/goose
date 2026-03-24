#include <goose/extension/core_functions/scalar/date_functions.h>
#include <goose/common/exception.h>
#include <goose/common/operator/cast_operators.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/main/client_context.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/transaction/meta_transaction.h>
#include <goose/planner/expression/bound_cast_expression.h>

namespace goose {

static timestamp_t GetTransactionTimestamp(ExpressionState &state) {
	return MetaTransaction::Get(state.GetContext()).start_timestamp;
}

static void CurrentTimestampFunction(DataChunk &input, ExpressionState &state, Vector &result) {
	D_ASSERT(input.ColumnCount() == 0);
	auto ts = GetTransactionTimestamp(state);
	auto val = Value::TIMESTAMPTZ(timestamp_tz_t(ts));
	result.Reference(val);
}

ScalarFunction GetCurrentTimestampFun::GetFunction() {
	ScalarFunction current_timestamp({}, LogicalType::TIMESTAMP_TZ, CurrentTimestampFunction);
	current_timestamp.SetStability(FunctionStability::CONSISTENT_WITHIN_QUERY);
	return current_timestamp;
}

} // namespace goose
