#include <goose/execution/operator/scan/physical_empty_result.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_empty_result.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalEmptyResult &op) {
	D_ASSERT(op.children.size() == 0);
	return Make<PhysicalEmptyResult>(op.types, op.estimated_cardinality);
}

} // namespace goose
