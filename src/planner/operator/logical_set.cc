#include <goose/planner/operator/logical_set.h>

namespace goose {

idx_t LogicalSet::EstimateCardinality(ClientContext &context) {
	return 1;
}

} // namespace goose
