#include <goose/catalog/default/default_generator.h>
#include <goose/catalog/catalog_transaction.h>

namespace goose {

DefaultGenerator::DefaultGenerator(Catalog &catalog) : catalog(catalog), created_all_entries(false) {
}
DefaultGenerator::~DefaultGenerator() {
}

unique_ptr<CatalogEntry> DefaultGenerator::CreateDefaultEntry(ClientContext &context, const string &entry_name) {
	throw InternalException("CreateDefaultEntry with ClientContext called but not supported in this generator");
}

unique_ptr<CatalogEntry> DefaultGenerator::CreateDefaultEntry(CatalogTransaction transaction,
                                                              const string &entry_name) {
	if (!transaction.context) {
		// no context - cannot create default entry
		return nullptr;
	}
	return CreateDefaultEntry(*transaction.context, entry_name);
}

} // namespace goose
