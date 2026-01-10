#include "core_functions/aggregate/algebraic_functions.h"
#include <goose/common/types/null_value.h>
#include "core_functions/aggregate/algebraic/covar.h"

namespace goose {

AggregateFunction CovarPopFun::GetFunction() {
	return AggregateFunction::BinaryAggregate<CovarState, double, double, double, CovarPopOperation>(
	    LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE);
}

AggregateFunction CovarSampFun::GetFunction() {
	return AggregateFunction::BinaryAggregate<CovarState, double, double, double, CovarSampOperation>(
	    LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE);
}

} // namespace goose
