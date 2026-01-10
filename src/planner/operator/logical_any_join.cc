#include <goose/planner/operator/logical_any_join.h>

namespace goose {

LogicalAnyJoin::LogicalAnyJoin(JoinType type) : LogicalJoin(type, LogicalOperatorType::LOGICAL_ANY_JOIN) {
}

InsertionOrderPreservingMap<string> LogicalAnyJoin::ParamsToString() const {
	InsertionOrderPreservingMap<string> result;
	result["Condition"] = condition->ToString();
	SetParamsEstimatedCardinality(result);
	return result;
}

} // namespace goose
