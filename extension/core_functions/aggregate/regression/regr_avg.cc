#include <goose/common/exception.h>
#include <goose/common/vector_operations/vector_operations.h>
#include "core_functions/aggregate/regression_functions.h"
#include <goose/planner/expression/bound_aggregate_expression.h>
#include <goose/function/function_set.h>

namespace goose {

namespace {

struct RegrState {
	double sum;
	size_t count;
};

struct RegrAvgFunction {
	template <class STATE>
	static void Initialize(STATE &state) {
		state.sum = 0;
		state.count = 0;
	}

	template <class STATE, class OP>
	static void Combine(const STATE &source, STATE &target, AggregateInputData &) {
		target.sum += source.sum;
		target.count += source.count;
	}

	template <class T, class STATE>
	static void Finalize(STATE &state, T &target, AggregateFinalizeData &finalize_data) {
		if (state.count == 0) {
			finalize_data.ReturnNull();
		} else {
			target = state.sum / (double)state.count;
		}
	}
	static bool IgnoreNull() {
		return true;
	}
};
struct RegrAvgXFunction : RegrAvgFunction {
	template <class A_TYPE, class B_TYPE, class STATE, class OP>
	static void Operation(STATE &state, const A_TYPE &y, const B_TYPE &x, AggregateBinaryInput &idata) {
		state.sum += x;
		state.count++;
	}
};

struct RegrAvgYFunction : RegrAvgFunction {
	template <class A_TYPE, class B_TYPE, class STATE, class OP>
	static void Operation(STATE &state, const A_TYPE &y, const B_TYPE &x, AggregateBinaryInput &idata) {
		state.sum += y;
		state.count++;
	}
};

} // namespace

AggregateFunction RegrAvgxFun::GetFunction() {
	return AggregateFunction::BinaryAggregate<RegrState, double, double, double, RegrAvgXFunction>(
	    LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE);
}

AggregateFunction RegrAvgyFun::GetFunction() {
	return AggregateFunction::BinaryAggregate<RegrState, double, double, double, RegrAvgYFunction>(
	    LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE);
}

} // namespace goose
