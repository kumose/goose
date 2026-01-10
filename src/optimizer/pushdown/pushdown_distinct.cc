#include <goose/optimizer/filter_pushdown.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/operator/logical_distinct.h>

namespace goose {

unique_ptr<LogicalOperator> FilterPushdown::PushdownDistinct(unique_ptr<LogicalOperator> op) {
	D_ASSERT(op->type == LogicalOperatorType::LOGICAL_DISTINCT);
	auto &distinct = op->Cast<LogicalDistinct>();
	if (!distinct.order_by) {
		// regular DISTINCT - can just push down
		op->children[0] = Rewrite(std::move(op->children[0]));
		return op;
	}
	// no pushdown through DISTINCT ON (yet?)
	return FinishPushdown(std::move(op));
}

} // namespace goose
