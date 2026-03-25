#include <goose/planner/operator/logical_recursive_cte.h>

#include <goose/main/config.h>

namespace goose {

InsertionOrderPreservingMap<string> LogicalRecursiveCTE::ParamsToString() const {
	InsertionOrderPreservingMap<string> result;
	result["CTE Name"] = ctename;
	result["Table Index"] = StringUtil::Format("%llu", table_index);
	SetParamsEstimatedCardinality(result);
	return result;
}

vector<idx_t> LogicalRecursiveCTE::GetTableIndex() const {
	return vector<idx_t> {table_index};
}

string LogicalRecursiveCTE::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return LogicalOperator::GetName() + StringUtil::Format(" #%llu", table_index);
	}
#endif
	return LogicalOperator::GetName();
}

} // namespace goose
