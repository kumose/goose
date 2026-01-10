#include <goose/execution/operator/join/physical_blockwise_nl_join.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_any_join.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalAnyJoin &op) {
	// Visit the child nodes.
	D_ASSERT(op.children.size() == 2);
	D_ASSERT(op.condition);
	auto &left = CreatePlan(*op.children[0]);
	auto &right = CreatePlan(*op.children[1]);

	// Create the blockwise NL join.
	return Make<PhysicalBlockwiseNLJoin>(op, left, right, std::move(op.condition), op.join_type,
	                                     op.estimated_cardinality);
}

} // namespace goose
