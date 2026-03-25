#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/execution/operator/persistent/physical_update.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_update.h>
#include <goose/catalog/goose_catalog.h>

namespace goose {

PhysicalOperator &GooseCatalog::PlanUpdate(ClientContext &context, PhysicalPlanGenerator &planner, LogicalUpdate &op,
                                          PhysicalOperator &plan) {
	auto &update = planner.Make<PhysicalUpdate>(
	    op.types, op.table, op.table.GetStorage(), op.columns, std::move(op.expressions), std::move(op.bound_defaults),
	    std::move(op.bound_constraints), op.estimated_cardinality, op.return_chunk);
	auto &cast_update = update.Cast<PhysicalUpdate>();
	cast_update.update_is_del_and_insert = op.update_is_del_and_insert;
	cast_update.children.push_back(plan);
	return update;
}

PhysicalOperator &Catalog::PlanUpdate(ClientContext &context, PhysicalPlanGenerator &planner, LogicalUpdate &op) {
	auto &plan = planner.CreatePlan(*op.children[0]);
	return PlanUpdate(context, planner, op, plan);
}

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalUpdate &op) {
	D_ASSERT(op.children.size() == 1);
	dependencies.AddDependency(op.table);
	return op.table.catalog.PlanUpdate(context, *this, op);
}

} // namespace goose
