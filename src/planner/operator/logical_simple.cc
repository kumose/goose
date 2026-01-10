#include <goose/planner/operator/logical_simple.h>
#include <goose/parser/parsed_data/alter_info.h>
#include <goose/parser/parsed_data/attach_info.h>
#include <goose/parser/parsed_data/drop_info.h>
#include <goose/parser/parsed_data/load_info.h>
#include <goose/parser/parsed_data/transaction_info.h>
#include <goose/parser/parsed_data/vacuum_info.h>
#include <goose/parser/parsed_data/detach_info.h>

namespace goose {

idx_t LogicalSimple::EstimateCardinality(ClientContext &context) {
	return 1;
}

} // namespace goose
