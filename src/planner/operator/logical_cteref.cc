#include <goose/planner/operator/logical_cteref.h>

#include <goose/main/config.h>

namespace goose {

InsertionOrderPreservingMap<string> LogicalCTERef::ParamsToString() const {
	InsertionOrderPreservingMap<string> result;
	result["CTE Index"] = StringUtil::Format("%llu", cte_index);
	SetParamsEstimatedCardinality(result);
	return result;
}

vector<idx_t> LogicalCTERef::GetTableIndex() const {
	return vector<idx_t> {table_index};
}

string LogicalCTERef::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return LogicalOperator::GetName() + StringUtil::Format(" #%llu", table_index);
	}
#endif
	return LogicalOperator::GetName();
}

} // namespace goose
