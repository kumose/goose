#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_create_secret.h>
#include <goose/execution/operator/helper/physical_create_secret.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalCreateSecret &op) {
	return Make<PhysicalCreateSecret>(op.secret_input, op.estimated_cardinality);
}

} // namespace goose
