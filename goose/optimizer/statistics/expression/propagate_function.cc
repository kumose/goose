#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/expression/bound_function_expression.h>

namespace goose {

unique_ptr<BaseStatistics> StatisticsPropagator::PropagateExpression(BoundFunctionExpression &func,
                                                                     unique_ptr<Expression> &expr_ptr) {
	vector<BaseStatistics> stats;
	stats.reserve(func.children.size());
	for (idx_t i = 0; i < func.children.size(); i++) {
		auto stat = PropagateExpression(func.children[i]);
		if (!stat) {
			stats.push_back(BaseStatistics::CreateUnknown(func.children[i]->return_type));
		} else {
			stats.push_back(stat->Copy());
		}
	}
	if (!func.function.HasStatisticsCallback()) {
		return nullptr;
	}
	FunctionStatisticsInput input(func, func.bind_info.get(), stats, &expr_ptr);
	return func.function.GetStatisticsCallback()(context, input);
}

} // namespace goose
