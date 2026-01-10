#include <goose/execution/operator/schema/physical_create_function.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry/scalar_macro_catalog_entry.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalCreateFunction::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                         OperatorSourceInput &input) const {
	auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
	catalog.CreateFunction(context.client, *info);

	return SourceResultType::FINISHED;
}

} // namespace goose
