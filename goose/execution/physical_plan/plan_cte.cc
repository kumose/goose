#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/operator/scan/physical_column_data_scan.h>
#include <goose/execution/operator/set/physical_cte.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/expression/bound_reference_expression.h>
#include <goose/planner/operator/logical_cteref.h>
#include <goose/planner/operator/logical_materialized_cte.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalMaterializedCTE &op) {
	D_ASSERT(op.children.size() == 2);

	// Create the working_table that the PhysicalCTE will use for evaluation.
	auto working_table = make_shared_ptr<ColumnDataCollection>(context, op.children[0]->types);

	// Add the ColumnDataCollection to the context of this PhysicalPlanGenerator
	recursive_cte_tables[op.table_index] = working_table;
	materialized_ctes[op.table_index] = vector<const_reference<PhysicalOperator>>();

	// Create the plan for the left side. This is the materialization.
	auto &left = CreatePlan(*op.children[0]);
	// Initialize an empty vector to collect the scan operators.
	auto &right = CreatePlan(*op.children[1]);

	auto &cte = Make<PhysicalCTE>(op.ctename, op.table_index, right.types, left, right, op.estimated_cardinality);
	auto &cast_cte = cte.Cast<PhysicalCTE>();
	cast_cte.working_table = working_table;
	cast_cte.cte_scans = materialized_ctes[op.table_index];
	return cte;
}

} // namespace goose
