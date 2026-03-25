#include <goose/extension/core_functions/aggregate/algebraic_functions.h>
#include <goose/extension/core_functions/aggregate/algebraic/covar.h>
#include <goose/extension/core_functions/aggregate/algebraic/stddev.h>
#include <goose/extension/core_functions/aggregate/algebraic/corr.h>
#include <goose/function/function_set.h>

namespace goose {

AggregateFunction CorrFun::GetFunction() {
	return AggregateFunction::BinaryAggregate<CorrState, double, double, double, CorrOperation>(
	    LogicalType::DOUBLE, LogicalType::DOUBLE, LogicalType::DOUBLE);
}
} // namespace goose
