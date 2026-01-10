#include <goose/catalog/catalog_entry/goose_index_entry.h>

#include <goose/storage/data_table.h>
#include <goose/catalog/catalog_entry/goose_table_entry.h>

namespace goose {

IndexDataTableInfo::IndexDataTableInfo(shared_ptr<DataTableInfo> info_p, const string &index_name_p)
    : info(std::move(info_p)), index_name(index_name_p) {
}

void GooseIndexEntry::Rollback(CatalogEntry &) {
	if (!info) {
		return;
	}
	if (!info->info) {
		return;
	}
	info->info->GetIndexes().RemoveIndex(name);
}

GooseIndexEntry::GooseIndexEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateIndexInfo &create_info,
                               TableCatalogEntry &table_p)
    : IndexCatalogEntry(catalog, schema, create_info), initial_index_size(0) {
	auto &table = table_p.Cast<GooseTableEntry>();
	auto &storage = table.GetStorage();
	info = make_shared_ptr<IndexDataTableInfo>(storage.GetDataTableInfo(), name);
}

GooseIndexEntry::GooseIndexEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateIndexInfo &create_info,
                               shared_ptr<IndexDataTableInfo> storage_info)
    : IndexCatalogEntry(catalog, schema, create_info), info(std::move(storage_info)), initial_index_size(0) {
}

unique_ptr<CatalogEntry> GooseIndexEntry::Copy(ClientContext &context) const {
	auto info_copy = GetInfo();
	auto &cast_info = info_copy->Cast<CreateIndexInfo>();

	auto result = make_uniq<GooseIndexEntry>(catalog, schema, cast_info, info);
	result->initial_index_size = initial_index_size;

	return std::move(result);
}

string GooseIndexEntry::GetSchemaName() const {
	return GetDataTableInfo().GetSchemaName();
}

string GooseIndexEntry::GetTableName() const {
	return GetDataTableInfo().GetTableName();
}

DataTableInfo &GooseIndexEntry::GetDataTableInfo() const {
	return *info->info;
}

void GooseIndexEntry::CommitDrop() {
	D_ASSERT(info);
	auto &indexes = GetDataTableInfo().GetIndexes();
	indexes.CommitDrop(name);
	indexes.RemoveIndex(name);
}

} // namespace goose
