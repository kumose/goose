#include <goose/planner/operator/logical_reset.h>

namespace goose {

idx_t LogicalReset::EstimateCardinality(ClientContext &context) {
	return 1;
}

} // namespace goose
