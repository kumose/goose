#include <goose/execution/operator/order/physical_top_n.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_top_n.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalTopN &op) {
	D_ASSERT(op.children.size() == 1);
	auto &plan = CreatePlan(*op.children[0]);
	auto &top_n =
	    Make<PhysicalTopN>(op.types, std::move(op.orders), NumericCast<idx_t>(op.limit), NumericCast<idx_t>(op.offset),
	                       std::move(op.dynamic_filter), op.estimated_cardinality);
	top_n.children.push_back(plan);
	return top_n;
}

} // namespace goose
