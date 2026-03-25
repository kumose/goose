#include <goose/execution/operator/filter/physical_filter.h>
#include <goose/execution/operator/projection/physical_projection.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/optimizer/matcher/expression_matcher.h>
#include <goose/planner/expression/bound_comparison_expression.h>
#include <goose/planner/expression/bound_constant_expression.h>
#include <goose/planner/expression/bound_reference_expression.h>
#include <goose/planner/operator/logical_filter.h>
#include <goose/planner/operator/logical_get.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalFilter &op) {
	D_ASSERT(op.children.size() == 1);
	reference<PhysicalOperator> plan = CreatePlan(*op.children[0]);
	if (!op.expressions.empty()) {
		// create a filter if there is anything to filter
		auto &filter = Make<PhysicalFilter>(plan.get().GetTypes(), std::move(op.expressions), op.estimated_cardinality);
		filter.children.push_back(plan);
		plan = filter;
	}
	if (op.HasProjectionMap()) {
		// there is a projection map, generate a physical projection
		vector<unique_ptr<Expression>> select_list;
		for (idx_t i = 0; i < op.projection_map.size(); i++) {
			select_list.push_back(make_uniq<BoundReferenceExpression>(op.types[i], op.projection_map[i]));
		}
		auto &proj = Make<PhysicalProjection>(op.types, std::move(select_list), op.estimated_cardinality);
		proj.children.push_back(plan);
		plan = proj;
	}
	return plan;
}

} // namespace goose
