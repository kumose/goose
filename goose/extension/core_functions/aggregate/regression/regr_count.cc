#include <goose/common/exception.h>
#include <goose/common/vector_operations/vector_operations.h>
#include <goose/extension/core_functions/aggregate/regression_functions.h>
#include <goose/planner/expression/bound_aggregate_expression.h>
#include <goose/extension/core_functions/aggregate/regression/regr_count.h>
#include <goose/function/function_set.h>

namespace goose {

AggregateFunction RegrCountFun::GetFunction() {
	auto regr_count = AggregateFunction::BinaryAggregate<size_t, double, double, uint32_t, RegrCountFunction>(
	    LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::UINTEGER);
	regr_count.name = "regr_count";
	regr_count.SetNullHandling(FunctionNullHandling::SPECIAL_HANDLING);
	return regr_count;
}

} // namespace goose
