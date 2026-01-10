#include <goose/execution/operator/schema/physical_drop.h>
#include <goose/main/client_data.h>
#include <goose/main/database_manager.h>
#include <goose/main/database.h>
#include <goose/main/client_context.h>
#include <goose/main/secret/secret_manager.h>
#include <goose/catalog/catalog_search_path.h>
#include <goose/main/settings.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
SourceResultType PhysicalDrop::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                               OperatorSourceInput &input) const {
	switch (info->type) {
	case CatalogType::PREPARED_STATEMENT: {
		// DEALLOCATE silently ignores errors
		auto &statements = ClientData::Get(context.client).prepared_statements;
		auto stmt_iter = statements.find(info->name);
		if (stmt_iter != statements.end()) {
			statements.erase(stmt_iter);
		}
		break;
	}
	case CatalogType::SCHEMA_ENTRY: {
		auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
		catalog.DropEntry(context.client, *info);

		// Check if the dropped schema was set as the current schema
		auto &client_data = ClientData::Get(context.client);
		auto &default_entry = client_data.catalog_search_path->GetDefault();
		auto &current_catalog = default_entry.catalog;
		auto &current_schema = default_entry.schema;
		D_ASSERT(info->name != DEFAULT_SCHEMA);

		if (info->catalog == current_catalog && current_schema == info->name) {
			// Reset the schema to default
			SchemaSetting::SetLocal(context.client, DEFAULT_SCHEMA);
		}
		break;
	}
	case CatalogType::SECRET_ENTRY: {
		// Note: the schema param is used to optionally pass the storage to drop from
		D_ASSERT(info->extra_drop_info);
		auto &extra_info = info->extra_drop_info->Cast<ExtraDropSecretInfo>();
		SecretManager::Get(context.client)
		    .DropSecretByName(context.client, info->name, info->if_not_found, extra_info.persist_mode,
		                      extra_info.secret_storage);
		break;
	}
	default: {
		auto &catalog = Catalog::GetCatalog(context.client, info->catalog);
		catalog.DropEntry(context.client, *info);
		break;
	}
	}

	return SourceResultType::FINISHED;
}

} // namespace goose
