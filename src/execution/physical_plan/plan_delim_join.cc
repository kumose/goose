#include <goose/common/enum_util.h>
#include <goose/execution/operator/aggregate/physical_hash_aggregate.h>
#include <goose/execution/operator/join/physical_hash_join.h>
#include <goose/execution/operator/join/physical_left_delim_join.h>
#include <goose/execution/operator/join/physical_right_delim_join.h>
#include <goose/execution/operator/projection/physical_projection.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/expression/bound_reference_expression.h>

#include <goose/execution/operator/scan/physical_column_data_scan.h>

namespace goose {

static void GatherDelimScans(PhysicalOperator &op, vector<const_reference<PhysicalOperator>> &delim_scans,
                             idx_t delim_index) {
	if (op.type == PhysicalOperatorType::DELIM_SCAN) {
		auto &scan = op.Cast<PhysicalColumnDataScan>();
		scan.delim_index = optional_idx(delim_index);
		delim_scans.push_back(op);
	}
	for (auto &child : op.children) {
		GatherDelimScans(child, delim_scans, delim_index);
	}
}

PhysicalOperator &PhysicalPlanGenerator::PlanDelimJoin(LogicalComparisonJoin &op) {
	// first create the underlying join
	auto &plan = PlanComparisonJoin(op);
	// this should create a join, not a cross product
	D_ASSERT(plan.type != PhysicalOperatorType::CROSS_PRODUCT);
	// duplicate eliminated join
	// first gather the scans on the duplicate eliminated data set from the delim side
	const idx_t delim_idx = op.delim_flipped ? 0 : 1;
	vector<const_reference<PhysicalOperator>> delim_scans;
	GatherDelimScans(plan.children[delim_idx], delim_scans, ++this->delim_index);
	if (delim_scans.empty()) {
		// no duplicate eliminated scans in the delim side!
		// in this case we don't need to create a delim join
		// just push the normal join
		return plan;
	}
	vector<LogicalType> delim_types;
	vector<unique_ptr<Expression>> distinct_groups, distinct_expressions;
	for (auto &delim_expr : op.duplicate_eliminated_columns) {
		D_ASSERT(delim_expr->GetExpressionType() == ExpressionType::BOUND_REF);
		auto &bound_ref = delim_expr->Cast<BoundReferenceExpression>();
		delim_types.push_back(bound_ref.return_type);
		distinct_groups.push_back(make_uniq<BoundReferenceExpression>(bound_ref.return_type, bound_ref.index));
	}

	// we still have to create the DISTINCT clause that is used to generate the duplicate eliminated chunk
	auto &distinct =
	    Make<PhysicalHashAggregate>(context, delim_types, std::move(distinct_expressions), std::move(distinct_groups),
	                                delim_scans[0].get().estimated_cardinality);

	// Create the duplicate eliminated join.
	if (op.delim_flipped) {
		return Make<PhysicalRightDelimJoin>(*this, op.types, plan, distinct, delim_scans, op.estimated_cardinality,
		                                    optional_idx(this->delim_index));
	}
	return Make<PhysicalLeftDelimJoin>(*this, op.types, plan, distinct, delim_scans, op.estimated_cardinality,
	                                   optional_idx(this->delim_index));
}

} // namespace goose
