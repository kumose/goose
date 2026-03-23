#include <goose/function/scalar/system_functions.h>
#include <goose/execution/expression_executor.h>
#include <goose/main/client_data.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/common/types/value.h>

#include <goose/utility/utf8proc.h>

namespace goose {

namespace {

struct CurrentTransactionIdData : FunctionData {
	explicit CurrentTransactionIdData(Value transaction_id_p) : transaction_id(std::move(transaction_id_p)) {
	}
	Value transaction_id;

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<CurrentTransactionIdData>(transaction_id);
	}
	bool Equals(const FunctionData &other_p) const override {
		return transaction_id == other_p.Cast<CurrentTransactionIdData>().transaction_id;
	}
};

unique_ptr<FunctionData> CurrentTransactionIdBind(ClientContext &context, ScalarFunction &bound_function,
                                                  vector<unique_ptr<Expression>> &arguments) {
	Value transaction_id;
	if (context.transaction.HasActiveTransaction()) {
		transaction_id = Value::UBIGINT(context.transaction.ActiveTransaction().global_transaction_id);
	} else {
		transaction_id = Value();
	}
	return make_uniq<CurrentTransactionIdData>(transaction_id);
}

void CurrentTransactionIdFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = state.expr.Cast<BoundFunctionExpression>();
	const auto &info = func_expr.bind_info->Cast<CurrentTransactionIdData>();
	result.Reference(info.transaction_id);
}

} // namespace

ScalarFunction CurrentTransactionId::GetFunction() {
	return ScalarFunction({}, LogicalType::UBIGINT, CurrentTransactionIdFunction, CurrentTransactionIdBind, nullptr,
	                      nullptr, nullptr, LogicalType(LogicalTypeId::INVALID), FunctionStability::VOLATILE);
}

} // namespace goose
