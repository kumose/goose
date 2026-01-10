#include <goose/execution/operator/schema/physical_detach.h>
#include <goose/parser/parsed_data/detach_info.h>
#include <goose/catalog/catalog.h>
#include <goose/main/database_manager.h>
#include <goose/main/attached_database.h>
#include <goose/main/database.h>
#include <goose/storage/storage_extension.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalDetach::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                 OperatorSourceInput &input) const {
	auto &db_manager = DatabaseManager::Get(context.client);
	db_manager.DetachDatabase(context.client, info->name, info->if_not_found);

	return SourceResultType::FINISHED;
}

} // namespace goose
