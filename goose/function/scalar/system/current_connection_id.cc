#include <goose/function/scalar/system_functions.h>
#include <goose/execution/expression_executor.h>
#include <goose/main/client_data.h>
#include <goose/planner/expression/bound_function_expression.h>

#include <goose/utility/utf8proc.h>

namespace goose {

namespace {

struct CurrentConnectionIdData : FunctionData {
	explicit CurrentConnectionIdData(Value connection_id_p) : connection_id(std::move(connection_id_p)) {
	}
	Value connection_id;

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<CurrentConnectionIdData>(connection_id);
	}
	bool Equals(const FunctionData &other_p) const override {
		return connection_id == other_p.Cast<CurrentConnectionIdData>().connection_id;
	}
};

unique_ptr<FunctionData> CurrentConnectionIdBind(ClientContext &context, ScalarFunction &bound_function,
                                                 vector<unique_ptr<Expression>> &arguments) {
	return make_uniq<CurrentConnectionIdData>(Value::UBIGINT(context.GetConnectionId()));
}

void CurrentConnectionIdFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = state.expr.Cast<BoundFunctionExpression>();
	const auto &info = func_expr.bind_info->Cast<CurrentConnectionIdData>();
	result.Reference(info.connection_id);
}

} // namespace

ScalarFunction CurrentConnectionId::GetFunction() {
	return ScalarFunction({}, LogicalType::UBIGINT, CurrentConnectionIdFunction, CurrentConnectionIdBind, nullptr,
	                      nullptr, nullptr, LogicalType(LogicalTypeId::INVALID), FunctionStability::VOLATILE);
}

} // namespace goose
