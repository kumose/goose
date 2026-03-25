#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/expression/bound_columnref_expression.h>

namespace goose {

unique_ptr<BaseStatistics> StatisticsPropagator::PropagateExpression(BoundColumnRefExpression &colref,
                                                                     unique_ptr<Expression> &expr_ptr) {
	auto stats = statistics_map.find(colref.binding);
	if (stats == statistics_map.end()) {
		return nullptr;
	}
	return stats->second->ToUnique();
}

} // namespace goose
