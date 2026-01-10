#include <goose/planner/operator/logical_dummy_scan.h>

#include <goose/main/config.h>

namespace goose {

vector<idx_t> LogicalDummyScan::GetTableIndex() const {
	return vector<idx_t> {table_index};
}

string LogicalDummyScan::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return LogicalOperator::GetName() + StringUtil::Format(" #%llu", table_index);
	}
#endif
	return LogicalOperator::GetName();
}

} // namespace goose
