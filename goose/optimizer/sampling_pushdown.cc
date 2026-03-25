#include <goose/optimizer/sampling_pushdown.h>
#include <goose/planner/operator/logical_get.h>
#include <goose/planner/operator/logical_sample.h>
#include <goose/common/types/value.h>
namespace goose {

unique_ptr<LogicalOperator> SamplingPushdown::Optimize(unique_ptr<LogicalOperator> op) {
	if (op->type == LogicalOperatorType::LOGICAL_SAMPLE &&
	    op->Cast<LogicalSample>().sample_options->method == SampleMethod::SYSTEM_SAMPLE &&
	    op->Cast<LogicalSample>().sample_options->is_percentage && !op->children.empty() &&
	    op->children[0]->type == LogicalOperatorType::LOGICAL_GET &&
	    op->children[0]->Cast<LogicalGet>().function.sampling_pushdown && op->children[0]->children.empty()) {
		auto &get = op->children[0]->Cast<LogicalGet>();
		// set sampling option
		get.extra_info.sample_options = std::move(op->Cast<LogicalSample>().sample_options);
		op = std::move(op->children[0]);
	}
	for (auto &child : op->children) {
		child = Optimize(std::move(child));
	}
	return op;
}

} // namespace goose
