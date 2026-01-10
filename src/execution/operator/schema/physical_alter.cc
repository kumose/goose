#include <goose/execution/operator/schema/physical_alter.h>
#include <goose/parser/parsed_data/alter_table_info.h>
#include <goose/parser/parsed_data/alter_database_info.h>
#include <goose/main/database_manager.h>
#include <goose/catalog/catalog.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalAlter::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                OperatorSourceInput &input) const {
	if (info->type == AlterType::ALTER_DATABASE) {
		auto &db_info = info->Cast<AlterDatabaseInfo>();
		auto &db_manager = DatabaseManager::Get(context.client);
		db_manager.Alter(context.client, db_info);
	} else {
		auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
		catalog.Alter(context.client, *info);
	}
	return SourceResultType::FINISHED;
}

} // namespace goose
