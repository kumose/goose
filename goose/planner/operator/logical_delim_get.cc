#include <goose/planner/operator/logical_delim_get.h>

#include <goose/main/config.h>

namespace goose {

vector<idx_t> LogicalDelimGet::GetTableIndex() const {
	return vector<idx_t> {table_index};
}

string LogicalDelimGet::GetName() const {
#ifdef DEBUG
	if (DBConfigOptions::debug_print_bindings) {
		return LogicalOperator::GetName() + StringUtil::Format(" #%llu", table_index);
	}
#endif
	return LogicalOperator::GetName();
}

} // namespace goose
