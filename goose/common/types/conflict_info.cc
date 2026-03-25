#include <goose/common/types/constraint_conflict_info.h>
#include <goose/storage/index.h>

namespace goose {

bool ConflictInfo::ConflictTargetMatches(Index &index) const {
	if (only_check_unique && !index.IsUnique()) {
		// We only support ON CONFLICT for PRIMARY KEY/UNIQUE constraints.
		return false;
	}
	if (column_ids.empty()) {
		return true;
	}
	return column_ids == index.GetColumnIdSet();
}

} // namespace goose
