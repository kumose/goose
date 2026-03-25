#include <goose/execution/operator/order/physical_order.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_order.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalOrder &op) {
	D_ASSERT(op.children.size() == 1);

	auto &plan = CreatePlan(*op.children[0]);
	if (op.orders.empty()) {
		return plan;
	}

	vector<idx_t> projection_map;
	if (op.HasProjectionMap()) {
		projection_map = std::move(op.projection_map);
	} else {
		for (idx_t i = 0; i < plan.types.size(); i++) {
			projection_map.push_back(i);
		}
	}
	auto &order =
	    Make<PhysicalOrder>(op.types, std::move(op.orders), std::move(projection_map), op.estimated_cardinality);
	order.children.push_back(plan);
	return order;
}

} // namespace goose
