#include <goose/execution/base_aggregate_hashtable.h>
#include <goose/planner/expression/bound_aggregate_expression.h>
#include <goose/storage/buffer_manager.h>

namespace goose {

BaseAggregateHashTable::BaseAggregateHashTable(ClientContext &context, Allocator &allocator,
                                               const vector<AggregateObject> &aggregates,
                                               vector<LogicalType> payload_types_p)
    : allocator(allocator), buffer_manager(BufferManager::GetBufferManager(context)),
      layout_ptr(make_shared_ptr<TupleDataLayout>()), payload_types(std::move(payload_types_p)) {
	filter_set.Initialize(context, aggregates, payload_types);
}

} // namespace goose
