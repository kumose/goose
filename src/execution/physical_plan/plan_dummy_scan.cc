#include <goose/execution/operator/scan/physical_dummy_scan.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_dummy_scan.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalDummyScan &op) {
	D_ASSERT(op.children.empty());
	return Make<PhysicalDummyScan>(op.types, op.estimated_cardinality);
}

} // namespace goose
