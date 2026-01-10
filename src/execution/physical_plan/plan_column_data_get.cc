#include <goose/execution/operator/scan/physical_column_data_scan.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_column_data_get.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalColumnDataGet &op) {
	D_ASSERT(op.children.empty());
	D_ASSERT(op.collection);
	return Make<PhysicalColumnDataScan>(op.types, PhysicalOperatorType::COLUMN_DATA_SCAN, op.estimated_cardinality,
	                                    std::move(op.collection));
}

} // namespace goose
