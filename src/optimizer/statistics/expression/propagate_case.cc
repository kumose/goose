#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/expression/bound_case_expression.h>

namespace goose {

unique_ptr<BaseStatistics> StatisticsPropagator::PropagateExpression(BoundCaseExpression &bound_case,
                                                                     unique_ptr<Expression> &expr_ptr) {
	// propagate in all the children
	auto result_stats = PropagateExpression(bound_case.else_expr);
	for (auto &case_check : bound_case.case_checks) {
		PropagateExpression(case_check.when_expr);
		auto then_stats = PropagateExpression(case_check.then_expr);
		if (!then_stats) {
			result_stats.reset();
		} else if (result_stats) {
			result_stats->Merge(*then_stats);
		}
	}
	return result_stats;
}

} // namespace goose
