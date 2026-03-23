#include <goose/execution/operator/helper/physical_execute.h>

#include <goose/parallel/meta_pipeline.h>

namespace goose {

PhysicalExecute::PhysicalExecute(PhysicalPlan &physical_plan, PhysicalOperator &plan)
    : PhysicalOperator(physical_plan, PhysicalOperatorType::EXECUTE, plan.types, idx_t(-1)), plan(plan) {
}

vector<const_reference<PhysicalOperator>> PhysicalExecute::GetChildren() const {
	return {plan};
}

void PhysicalExecute::BuildPipelines(Pipeline &current, MetaPipeline &meta_pipeline) {
	// EXECUTE statement: build pipeline on child
	meta_pipeline.Build(plan);
}

} // namespace goose
