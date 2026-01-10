#include <goose/planner/operator/logical_prepare.h>

namespace goose {

idx_t LogicalPrepare::EstimateCardinality(ClientContext &context) {
	return 1;
}

} // namespace goose
