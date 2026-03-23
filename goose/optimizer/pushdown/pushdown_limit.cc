#include <goose/optimizer/filter_pushdown.h>
#include <goose/optimizer/optimizer.h>
#include <goose/planner/expression/bound_columnref_expression.h>
#include <goose/planner/operator/logical_empty_result.h>
#include <goose/planner/operator/logical_limit.h>

namespace goose {

unique_ptr<LogicalOperator> FilterPushdown::PushdownLimit(unique_ptr<LogicalOperator> op) {
	auto &limit = op->Cast<LogicalLimit>();

	if (limit.limit_val.Type() == LimitNodeType::CONSTANT_VALUE && limit.limit_val.GetConstantValue() == 0) {
		return make_uniq<LogicalEmptyResult>(std::move(op));
	}

	return FinishPushdown(std::move(op));
}

} // namespace goose
