
#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/operator/logical_order.h>

namespace goose {

unique_ptr<NodeStatistics> StatisticsPropagator::PropagateStatistics(LogicalOrder &order,
                                                                     unique_ptr<LogicalOperator> &node_ptr) {
	// first propagate to the child
	node_stats = PropagateStatistics(order.children[0]);

	// then propagate to each of the order expressions
	for (auto &bound_order : order.orders) {
		bound_order.stats = PropagateExpression(bound_order.expression);
	}
	return std::move(node_stats);
}

} // namespace goose
