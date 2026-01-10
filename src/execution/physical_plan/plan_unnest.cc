#include <goose/execution/operator/projection/physical_unnest.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_unnest.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalUnnest &op) {
	D_ASSERT(op.children.size() == 1);
	auto &plan = CreatePlan(*op.children[0]);
	auto &unnest = Make<PhysicalUnnest>(op.types, std::move(op.expressions), op.estimated_cardinality);
	unnest.children.push_back(plan);
	return unnest;
}

} // namespace goose
