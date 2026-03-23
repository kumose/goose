#include <goose/execution/operator/join/physical_cross_product.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_cross_product.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalCrossProduct &op) {
	D_ASSERT(op.children.size() == 2);
	auto &left = CreatePlan(*op.children[0]);
	auto &right = CreatePlan(*op.children[1]);
	return Make<PhysicalCrossProduct>(op.types, left, right, op.estimated_cardinality);
}

} // namespace goose
