#include <goose/function/scalar/generic_functions.h>
#include <goose/common/exception.h>
#include <goose/execution/expression_executor.h>
#include <goose/planner/expression/bound_constant_expression.h>
#include <goose/transaction/meta_transaction.h>

namespace goose {

namespace {
struct GetVariableBindData : FunctionData {
	explicit GetVariableBindData(Value value_p) : value(std::move(value_p)) {
	}

	Value value;

	bool Equals(const FunctionData &other_p) const override {
		const auto &other = other_p.Cast<GetVariableBindData>();
		return Value::NotDistinctFrom(value, other.value);
	}

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<GetVariableBindData>(value);
	}
};

unique_ptr<FunctionData> GetVariableBind(ClientContext &context, ScalarFunction &function,
                                         vector<unique_ptr<Expression>> &arguments) {
	if (arguments[0]->HasParameter() || arguments[0]->return_type.id() == LogicalTypeId::UNKNOWN) {
		throw ParameterNotResolvedException();
	}
	if (!arguments[0]->IsFoldable()) {
		throw NotImplementedException("getvariable requires a constant input");
	}
	Value value;
	auto variable_name = ExpressionExecutor::EvaluateScalar(context, *arguments[0]);
	if (!variable_name.IsNull()) {
		ClientConfig::GetConfig(context).GetUserVariable(variable_name.ToString(), value);
	}
	function.SetReturnType(value.type());
	return make_uniq<GetVariableBindData>(std::move(value));
}

unique_ptr<Expression> BindGetVariableExpression(FunctionBindExpressionInput &input) {
	if (!input.bind_data) {
		// unknown type
		throw InternalException("input.bind_data should be set");
	}
	auto &bind_data = input.bind_data->Cast<GetVariableBindData>();
	// emit a constant expression
	return make_uniq<BoundConstantExpression>(bind_data.value);
}

} // namespace

ScalarFunction GetVariableFun::GetFunction() {
	ScalarFunction getvar("getvariable", {LogicalType::VARCHAR}, LogicalType::ANY, nullptr, GetVariableBind, nullptr);
	getvar.SetBindExpressionCallback(BindGetVariableExpression);
	return getvar;
}

} // namespace goose
