#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_set.h>
#include <goose/execution/operator/helper/physical_set.h>
#include <goose/execution/operator/helper/physical_set_variable.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalSet &op) {
	// Set a config setting.
	if (op.children.empty()) {
		return Make<PhysicalSet>(op.name, op.value, op.scope, op.estimated_cardinality);
	}

	// Set a variable.
	auto &plan = CreatePlan(*op.children[0]);
	auto &set_variable = Make<PhysicalSetVariable>(std::move(op.name), op.estimated_cardinality);
	set_variable.children.push_back(plan);
	return set_variable;
}

} // namespace goose
