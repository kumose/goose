#include <goose/catalog/catalog_entry/copy_function_catalog_entry.h>
#include <goose/parser/parsed_data/create_copy_function_info.h>

namespace goose {

constexpr const char *CopyFunctionCatalogEntry::Name;

CopyFunctionCatalogEntry::CopyFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema,
                                                   CreateCopyFunctionInfo &info)
    : StandardEntry(CatalogType::COPY_FUNCTION_ENTRY, schema, catalog, info.name), function(info.function) {
}

} // namespace goose
