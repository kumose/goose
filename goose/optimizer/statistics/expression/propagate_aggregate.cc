#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/expression/bound_aggregate_expression.h>

namespace goose {

unique_ptr<BaseStatistics> StatisticsPropagator::PropagateExpression(BoundAggregateExpression &aggr,
                                                                     unique_ptr<Expression> &expr_ptr) {
	vector<BaseStatistics> stats;
	stats.reserve(aggr.children.size());
	for (auto &child : aggr.children) {
		auto stat = PropagateExpression(child);
		if (!stat) {
			stats.push_back(BaseStatistics::CreateUnknown(child->return_type));
		} else {
			stats.push_back(stat->Copy());
		}
	}
	if (!aggr.function.HasStatisticsCallback()) {
		return nullptr;
	}
	AggregateStatisticsInput input(aggr.bind_info.get(), stats, node_stats.get());
	return aggr.function.GetStatisticsCallback()(context, aggr, input);
}

} // namespace goose
