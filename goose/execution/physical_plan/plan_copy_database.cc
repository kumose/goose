#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_copy_database.h>
#include <goose/execution/operator/persistent/physical_copy_database.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalCopyDatabase &op) {
	return Make<PhysicalCopyDatabase>(op.types, op.estimated_cardinality, std::move(op.info));
}

} // namespace goose
