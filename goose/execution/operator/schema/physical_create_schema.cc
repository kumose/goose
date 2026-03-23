#include <goose/execution/operator/schema/physical_create_schema.h>
#include <goose/catalog/catalog.h>
#include <goose/common/exception/binder_exception.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalCreateSchema::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                       OperatorSourceInput &input) const {
	auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
	if (catalog.IsSystemCatalog()) {
		throw BinderException("Cannot create schema in system catalog");
	}
	catalog.CreateSchema(context.client, *info);

	return SourceResultType::FINISHED;
}

} // namespace goose
