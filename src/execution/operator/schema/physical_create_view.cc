#include <goose/execution/operator/schema/physical_create_view.h>
#include <goose/catalog/catalog.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalCreateView::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                     OperatorSourceInput &input) const {
	auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
	catalog.CreateView(context.client, *info);

	return SourceResultType::FINISHED;
}

} // namespace goose
