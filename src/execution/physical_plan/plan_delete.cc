#include <goose/catalog/catalog_entry/goose_table_entry.h>
#include <goose/execution/operator/persistent/physical_delete.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/expression/bound_reference_expression.h>
#include <goose/planner/operator/logical_delete.h>
#include <goose/catalog/goose_catalog.h>

namespace goose {

PhysicalOperator &GooseCatalog::PlanDelete(ClientContext &context, PhysicalPlanGenerator &planner, LogicalDelete &op,
                                          PhysicalOperator &plan) {
	// Get the row_id column index.
	auto &bound_ref = op.expressions[0]->Cast<BoundReferenceExpression>();
	auto &del = planner.Make<PhysicalDelete>(op.types, op.table, op.table.GetStorage(), std::move(op.bound_constraints),
	                                         bound_ref.index, op.estimated_cardinality, op.return_chunk);
	del.children.push_back(plan);
	return del;
}

PhysicalOperator &Catalog::PlanDelete(ClientContext &context, PhysicalPlanGenerator &planner, LogicalDelete &op) {
	auto &plan = planner.CreatePlan(*op.children[0]);
	return PlanDelete(context, planner, op, plan);
}

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalDelete &op) {
	D_ASSERT(op.children.size() == 1);

	dependencies.AddDependency(op.table);
	return op.table.catalog.PlanDelete(context, *this, op);
}

} // namespace goose
