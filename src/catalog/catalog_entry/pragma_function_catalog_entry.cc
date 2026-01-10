#include <goose/catalog/catalog_entry/pragma_function_catalog_entry.h>
#include <goose/parser/parsed_data/create_pragma_function_info.h>

namespace goose {
constexpr const char *PragmaFunctionCatalogEntry::Name;

PragmaFunctionCatalogEntry::PragmaFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema,
                                                       CreatePragmaFunctionInfo &info)
    : FunctionEntry(CatalogType::PRAGMA_FUNCTION_ENTRY, catalog, schema, info), functions(std::move(info.functions)) {
}

} // namespace goose
