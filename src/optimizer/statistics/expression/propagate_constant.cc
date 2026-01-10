#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/expression/bound_constant_expression.h>
#include <goose/storage/statistics/distinct_statistics.h>
#include <goose/storage/statistics/list_stats.h>
#include <goose/storage/statistics/struct_stats.h>

namespace goose {

unique_ptr<BaseStatistics> StatisticsPropagator::StatisticsFromValue(const Value &input) {
	return BaseStatistics::FromConstant(input).ToUnique();
}

unique_ptr<BaseStatistics> StatisticsPropagator::PropagateExpression(BoundConstantExpression &constant,
                                                                     unique_ptr<Expression> &expr_ptr) {
	return StatisticsFromValue(constant.value);
}

} // namespace goose
