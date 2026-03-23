#include <goose/planner/operator/logical_pragma.h>

namespace goose {

idx_t LogicalPragma::EstimateCardinality(ClientContext &context) {
	return 1;
}

} // namespace goose
