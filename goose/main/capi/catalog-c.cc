#include <goose/common/type_visitor.h>
#include <goose/common/types.h>
#include <goose/common/helper.h>
#include <goose/main/capi/capi_internal.h>
#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry.h>

namespace goose {
namespace {

struct CCatalogWrapper {
	Catalog &catalog;
	string catalog_type;
	CCatalogWrapper(Catalog &catalog, const string &catalog_type) : catalog(catalog), catalog_type(catalog_type) {
	}
};

struct CCatalogEntryWrapper {
	CatalogEntry &entry;
	CCatalogEntryWrapper(CatalogEntry &entry) : entry(entry) {
	}
};

CatalogType CatalogTypeFromC(goose_catalog_entry_type type) {
	switch (type) {
	case GOOSE_CATALOG_ENTRY_TYPE_TABLE:
		return CatalogType::TABLE_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_SCHEMA:
		return CatalogType::SCHEMA_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_VIEW:
		return CatalogType::VIEW_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_INDEX:
		return CatalogType::INDEX_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_PREPARED_STATEMENT:
		return CatalogType::PREPARED_STATEMENT;
	case GOOSE_CATALOG_ENTRY_TYPE_SEQUENCE:
		return CatalogType::SEQUENCE_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_COLLATION:
		return CatalogType::COLLATION_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_TYPE:
		return CatalogType::TYPE_ENTRY;
	case GOOSE_CATALOG_ENTRY_TYPE_DATABASE:
		return CatalogType::DATABASE_ENTRY;
	default:
		return CatalogType::INVALID;
	}
}

goose_catalog_entry_type CatalogTypeToC(CatalogType type) {
	switch (type) {
	case CatalogType::TABLE_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_TABLE;
	case CatalogType::SCHEMA_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_SCHEMA;
	case CatalogType::VIEW_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_VIEW;
	case CatalogType::INDEX_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_INDEX;
	case CatalogType::PREPARED_STATEMENT:
		return GOOSE_CATALOG_ENTRY_TYPE_PREPARED_STATEMENT;
	case CatalogType::SEQUENCE_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_SEQUENCE;
	case CatalogType::COLLATION_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_COLLATION;
	case CatalogType::TYPE_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_TYPE;
	case CatalogType::DATABASE_ENTRY:
		return GOOSE_CATALOG_ENTRY_TYPE_DATABASE;
	default:
		return GOOSE_CATALOG_ENTRY_TYPE_INVALID;
	}
}

} // namespace
} // namespace goose

//----------------------------------------------------------------------------------------------------------------------
// Catalog
//----------------------------------------------------------------------------------------------------------------------
goose_catalog goose_client_context_get_catalog(goose_client_context context, const char *name) {
	if (!context || !name || strlen(name) == 0) {
		return nullptr;
	}

	auto &context_ref = *reinterpret_cast<goose::CClientContextWrapper *>(context);
	if (!context_ref.context.transaction.HasActiveTransaction()) {
		return nullptr;
	}

	auto catalog_ptr = goose::Catalog::GetCatalogEntry(context_ref.context, name);

	if (!catalog_ptr) {
		return nullptr;
	}

	auto &catalog_ref = *catalog_ptr;
	auto catalog_wrapper = new goose::CCatalogWrapper(catalog_ref, catalog_ref.GetCatalogType());
	return reinterpret_cast<goose_catalog>(catalog_wrapper);
}

void goose_destroy_catalog(goose_catalog *catalog) {
	if (!catalog || !*catalog) {
		return;
	}
	auto catalog_ptr = reinterpret_cast<goose::CCatalogWrapper *>(*catalog);
	delete catalog_ptr;
	*catalog = nullptr;
}

const char *goose_catalog_get_type_name(goose_catalog catalog) {
	if (!catalog) {
		return nullptr;
	}
	auto &catalog_ref = *reinterpret_cast<goose::CCatalogWrapper *>(catalog);
	return catalog_ref.catalog_type.c_str();
}

goose_catalog_entry goose_catalog_get_entry(goose_catalog catalog, goose_client_context context,
                                              goose_catalog_entry_type entry_type, const char *schema_name,
                                              const char *entry_name) {
	if (!catalog || !context || !schema_name || !entry_name) {
		return nullptr;
	}

	auto &catalog_ref = *reinterpret_cast<goose::CCatalogWrapper *>(catalog);
	auto &context_ref = *reinterpret_cast<goose::CClientContextWrapper *>(context);

	auto entry = catalog_ref.catalog.GetEntry(context_ref.context, goose::CatalogTypeFromC(entry_type), schema_name,
	                                          entry_name, goose::OnEntryNotFound::RETURN_NULL);

	if (!entry) {
		return nullptr;
	}

	return reinterpret_cast<goose_catalog_entry>(new goose::CCatalogEntryWrapper(*entry));
}

//----------------------------------------------------------------------------------------------------------------------
// Catalog Entry
//----------------------------------------------------------------------------------------------------------------------

goose_catalog_entry_type goose_catalog_entry_get_type(goose_catalog_entry entry) {
	if (!entry) {
		return GOOSE_CATALOG_ENTRY_TYPE_INVALID;
	}

	auto &entry_ref = *reinterpret_cast<goose::CCatalogEntryWrapper *>(entry);
	return goose::CatalogTypeToC(entry_ref.entry.type);
}

const char *goose_catalog_entry_get_name(goose_catalog_entry entry) {
	if (!entry) {
		return nullptr;
	}
	auto &entry_ref = *reinterpret_cast<goose::CCatalogEntryWrapper *>(entry);
	return entry_ref.entry.name.c_str();
}

void goose_destroy_catalog_entry(goose_catalog_entry *entry) {
	if (!entry || !*entry) {
		return;
	}
	auto entry_ptr = reinterpret_cast<goose::CCatalogEntryWrapper *>(*entry);
	delete entry_ptr;
	*entry = nullptr;
}
