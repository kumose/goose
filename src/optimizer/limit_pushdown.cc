#include <goose/optimizer/limit_pushdown.h>

#include <goose/planner/operator/logical_limit.h>
#include <goose/planner/operator/logical_projection.h>

namespace goose {

bool LimitPushdown::CanOptimize(goose::LogicalOperator &op) {
	if (op.type == LogicalOperatorType::LOGICAL_LIMIT &&
	    op.children[0]->type == LogicalOperatorType::LOGICAL_PROJECTION) {
		auto &limit = op.Cast<LogicalLimit>();

		if (limit.offset_val.Type() == LimitNodeType::EXPRESSION_PERCENTAGE ||
		    limit.offset_val.Type() == LimitNodeType::EXPRESSION_VALUE) {
			// Offset cannot be an expression
			return false;
		}

		if (limit.limit_val.Type() == LimitNodeType::CONSTANT_VALUE && limit.limit_val.GetConstantValue() < 8192) {
			// Push down only when limit value is smaller than 8192.
			// when physical_limit is introduced, it will end a parallel pipeline
			// restrict the limit value to be small so that remaining operations run fast without parallelization.
			return true;
		}
	}
	return false;
}

unique_ptr<LogicalOperator> LimitPushdown::Optimize(unique_ptr<LogicalOperator> op) {
	if (CanOptimize(*op)) {
		auto projection = std::move(op->children[0]);
		op->children[0] = std::move(projection->children[0]);
		projection->SetEstimatedCardinality(op->estimated_cardinality);
		projection->children[0] = std::move(op);
		swap(projection, op);
	}
	for (auto &child : op->children) {
		child = Optimize(std::move(child));
	}
	return op;
}

} // namespace goose
