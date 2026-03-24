#include <goose/extension/core_functions/aggregate/distributive_functions.h>
#include <goose/common/exception.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/planner/expression/bound_aggregate_expression.h>
#include <goose/function/function_set.h>

namespace goose {

namespace {

struct ProductState {
	bool empty;
	double val;
};

struct ProductFunction {
	template <class STATE>
	static void Initialize(STATE &state) {
		state.val = 1;
		state.empty = true;
	}

	template <class STATE, class OP>
	static void Combine(const STATE &source, STATE &target, AggregateInputData &) {
		target.val *= source.val;
		target.empty = target.empty && source.empty;
	}

	template <class T, class STATE>
	static void Finalize(STATE &state, T &target, AggregateFinalizeData &finalize_data) {
		if (state.empty) {
			finalize_data.ReturnNull();
			return;
		}
		target = state.val;
	}
	template <class INPUT_TYPE, class STATE, class OP>
	static void Operation(STATE &state, const INPUT_TYPE &input, AggregateUnaryInput &unary_input) {
		if (state.empty) {
			state.empty = false;
		}
		state.val *= input;
	}

	template <class INPUT_TYPE, class STATE, class OP>
	static void ConstantOperation(STATE &state, const INPUT_TYPE &input, AggregateUnaryInput &unary_input,
	                              idx_t count) {
		for (idx_t i = 0; i < count; i++) {
			Operation<INPUT_TYPE, STATE, OP>(state, input, unary_input);
		}
	}

	static bool IgnoreNull() {
		return true;
	}
};

} // namespace

AggregateFunction ProductFun::GetFunction() {
	return AggregateFunction::UnaryAggregate<ProductState, double, double, ProductFunction>(
	    LogicalType(LogicalTypeId::DOUBLE), LogicalType::DOUBLE);
}

} // namespace goose
