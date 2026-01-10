#include <goose/common/tree_renderer.h>
#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/operator/helper/physical_explain_analyze.h>
#include <goose/execution/operator/scan/physical_column_data_scan.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/main/client_context.h>
#include <goose/planner/operator/logical_explain.h>

namespace goose {

PhysicalOperator &PhysicalPlanGenerator::CreatePlan(LogicalExplain &op) {
	D_ASSERT(op.children.size() == 1);
	auto logical_plan_opt = op.children[0]->ToString(op.explain_format);
	auto &plan = CreatePlan(*op.children[0]);
	if (op.explain_type == ExplainType::EXPLAIN_ANALYZE) {
		auto &explain = Make<PhysicalExplainAnalyze>(op.types, op.explain_format);
		explain.children.push_back(plan);
		return explain;
	}

	// Format the plan and set the output of the EXPLAIN.
	op.physical_plan = plan.ToString(op.explain_format);
	vector<string> keys, values;
	switch (ClientConfig::GetConfig(context).explain_output_type) {
	case ExplainOutputType::OPTIMIZED_ONLY:
		keys = {"logical_opt"};
		values = {logical_plan_opt};
		break;
	case ExplainOutputType::PHYSICAL_ONLY:
		keys = {"physical_plan"};
		values = {op.physical_plan};
		break;
	default:
		keys = {"logical_plan", "logical_opt", "physical_plan"};
		values = {op.logical_plan_unopt, logical_plan_opt, op.physical_plan};
	}

	// Create a ColumnDataCollection from the output.
	auto &allocator = Allocator::Get(context);
	vector<LogicalType> plan_types {LogicalType::VARCHAR, LogicalType::VARCHAR};
	auto collection =
	    make_uniq<ColumnDataCollection>(context, plan_types, ColumnDataAllocatorType::IN_MEMORY_ALLOCATOR);

	DataChunk chunk;
	chunk.Initialize(allocator, op.types);
	for (idx_t i = 0; i < keys.size(); i++) {
		chunk.SetValue(0, chunk.size(), Value(keys[i]));
		chunk.SetValue(1, chunk.size(), Value(values[i]));
		chunk.SetCardinality(chunk.size() + 1);
		if (chunk.size() == STANDARD_VECTOR_SIZE) {
			collection->Append(chunk);
			chunk.Reset();
		}
	}
	collection->Append(chunk);

	// Output the result via a chunk scan.
	return Make<PhysicalColumnDataScan>(op.types, PhysicalOperatorType::COLUMN_DATA_SCAN, op.estimated_cardinality,
	                                    std::move(collection));
}

} // namespace goose
