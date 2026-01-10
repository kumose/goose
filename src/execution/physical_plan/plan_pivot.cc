#include <goose/execution/operator/projection/physical_pivot.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_pivot.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalPivot &op) {
	D_ASSERT(op.children.size() == 1);
	auto &plan = CreatePlan(*op.children[0]);
	return Make<PhysicalPivot>(std::move(op.types), plan, std::move(op.bound_pivot));
}

} // namespace goose
