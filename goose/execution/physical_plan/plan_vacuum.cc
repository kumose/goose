#include <goose/execution/operator/helper/physical_vacuum.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/operator/logical_vacuum.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalVacuum &op) {
	auto &vacuum = Make<PhysicalVacuum>(unique_ptr_cast<ParseInfo, VacuumInfo>(std::move(op.info)), op.table,
	                                    std::move(op.column_id_map), op.estimated_cardinality);
	if (!op.children.empty()) {
		auto &plan = CreatePlan(*op.children[0]);
		vacuum.children.push_back(plan);
	}
	return vacuum;
}

} // namespace goose
