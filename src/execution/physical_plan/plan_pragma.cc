#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_pragma.h>

#include <goose/execution/operator/helper/physical_pragma.h>
namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalPragma &op) {
	return Make<PhysicalPragma>(std::move(op.info), op.estimated_cardinality);
}

} // namespace goose
