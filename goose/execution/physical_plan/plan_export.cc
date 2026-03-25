#include <goose/execution/physical_plan_generator.h>
#include <goose/execution/operator/persistent/physical_export.h>
#include <goose/planner/operator/logical_export.h>
#include <goose/main/config.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalExport &op) {
	auto &export_op = Make<PhysicalExport>(op.types, op.function, std::move(op.copy_info), op.estimated_cardinality,
	                                       std::move(op.exported_tables));
	// Plan the underlying copy statements, if any.
	if (!op.children.empty()) {
		auto &plan = CreatePlan(*op.children[0]);
		export_op.children.push_back(plan);
	}
	return export_op;
}

} // namespace goose
