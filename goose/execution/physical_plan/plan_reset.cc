#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_reset.h>
#include <goose/execution/operator/helper/physical_reset.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalReset &op) {
	return Make<PhysicalReset>(op.name, op.scope, op.estimated_cardinality);
}

} // namespace goose
