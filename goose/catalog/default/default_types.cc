#include <goose/catalog/default/default_types.h>

#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog_entry/type_catalog_entry.h>
#include <goose/common/string_util.h>
#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/catalog/default/builtin_types/types.h>

namespace goose {

LogicalTypeId DefaultTypeGenerator::GetDefaultType(const string &name) {
	auto &internal_types = BUILTIN_TYPES;
	for (auto &type : internal_types) {
		if (StringUtil::CIEquals(name, type.name)) {
			return type.type;
		}
	}
	return LogicalType::INVALID;
}

DefaultTypeGenerator::DefaultTypeGenerator(Catalog &catalog, SchemaCatalogEntry &schema)
    : DefaultGenerator(catalog), schema(schema) {
}

unique_ptr<CatalogEntry> DefaultTypeGenerator::CreateDefaultEntry(ClientContext &context, const string &entry_name) {
	if (schema.name != DEFAULT_SCHEMA) {
		return nullptr;
	}
	auto type_id = GetDefaultType(entry_name);
	if (type_id == LogicalTypeId::INVALID) {
		return nullptr;
	}
	CreateTypeInfo info;
	info.name = entry_name;
	info.type = LogicalType(type_id);
	info.internal = true;
	info.temporary = true;
	return make_uniq_base<CatalogEntry, TypeCatalogEntry>(catalog, schema, info);
}

vector<string> DefaultTypeGenerator::GetDefaultEntries() {
	vector<string> result;
	if (schema.name != DEFAULT_SCHEMA) {
		return result;
	}
	auto &internal_types = BUILTIN_TYPES;
	for (auto &type : internal_types) {
		result.emplace_back(StringUtil::Lower(type.name));
	}
	return result;
}

} // namespace goose
