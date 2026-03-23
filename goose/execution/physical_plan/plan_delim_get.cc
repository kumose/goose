#include <goose/execution/operator/scan/physical_column_data_scan.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/operator/logical_delim_get.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalDelimGet &op) {
	// Create a PhysicalChunkScan without an owned_collection.
	// We'll add the collection later.
	D_ASSERT(op.children.empty());
	return Make<PhysicalColumnDataScan>(op.types, PhysicalOperatorType::DELIM_SCAN, op.estimated_cardinality, nullptr);
}

} // namespace goose
