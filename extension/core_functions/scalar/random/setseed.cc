#include "core_functions/scalar/random_functions.h"
#include <goose/common/exception.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/execution/expression_executor.h>
#include <goose/main/client_context.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/common/limits.h>
#include <goose/common/random_engine.h>

namespace goose {

namespace {

struct SetseedBindData : public FunctionData {
	//! The client context for the function call
	ClientContext &context;

	explicit SetseedBindData(ClientContext &context) : context(context) {
	}

	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<SetseedBindData>(context);
	}

	bool Equals(const FunctionData &other_p) const override {
		return true;
	}
};

void SetSeedFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = state.expr.Cast<BoundFunctionExpression>();
	auto &info = func_expr.bind_info->Cast<SetseedBindData>();
	auto &input = args.data[0];
	input.Flatten(args.size());

	auto input_seeds = FlatVector::GetData<double>(input);
	uint32_t half_max = NumericLimits<uint32_t>::Maximum() / 2;

	auto &random_engine = RandomEngine::Get(info.context);
	for (idx_t i = 0; i < args.size(); i++) {
		if (input_seeds[i] < -1.0 || input_seeds[i] > 1.0 || Value::IsNan(input_seeds[i])) {
			throw InvalidInputException("SETSEED accepts seed values between -1.0 and 1.0, inclusive");
		}
		auto norm_seed = LossyNumericCast<uint32_t>((input_seeds[i] + 1.0) * half_max);
		random_engine.SetSeed(norm_seed);
	}

	result.SetVectorType(VectorType::CONSTANT_VECTOR);
	ConstantVector::SetNull(result, true);
}

unique_ptr<FunctionData> SetSeedBind(ClientContext &context, ScalarFunction &bound_function,
                                     vector<unique_ptr<Expression>> &arguments) {
	return make_uniq<SetseedBindData>(context);
}

} // namespace

ScalarFunction SetseedFun::GetFunction() {
	ScalarFunction setseed("setseed", {LogicalType::DOUBLE}, LogicalType::SQLNULL, SetSeedFunction, SetSeedBind);
	setseed.SetVolatile();
	setseed.SetFallible();
	return setseed;
}

} // namespace goose
