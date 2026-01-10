#include <goose/execution/operator/schema/physical_create_sequence.h>
#include <goose/catalog/catalog.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalCreateSequence::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                         OperatorSourceInput &input) const {
	auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
	catalog.CreateSequence(context.client, *info);

	return SourceResultType::FINISHED;
}

} // namespace goose
