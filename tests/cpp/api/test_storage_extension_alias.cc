#include <goose/testing/catch.h>
#include <goose/testing/test_helpers.h>
#include <goose/main/config.h>
#include <goose/storage/storage_extension.h>
#include <goose/transaction/goose_transaction_manager.h>
#include <goose/catalog/goose_catalog.h>

using namespace goose;

// Test to see if extensions can be loaded with their normal name and aliases.

struct DummyStorageExtension : StorageExtension {
	DummyStorageExtension() {
		attach = [](optional_ptr<StorageExtensionInfo>, ClientContext &, AttachedDatabase &db, const string &,
		            AttachInfo &info, AttachOptions &) -> unique_ptr<Catalog> {
			return make_uniq_base<Catalog, GooseCatalog>(db);
		};
		create_transaction_manager = [](optional_ptr<StorageExtensionInfo>, AttachedDatabase &db,
		                                Catalog &) -> unique_ptr<TransactionManager> {
			return make_uniq<GooseTransactionManager>(db);
		};
	}
};

TEST_CASE("Test storage extension lookup full-name", "[api]") {
	DBConfig config;

	// Register a storage extension under its CANONICAL name "sqlite_scanner"
	// This mimics how the real sqlite_scanner extension registers itself
	// There is a hard-coded alias "sqlite" for "sqlite_scanner"
	config.storage_extensions["sqlite_scanner"] = make_uniq<DummyStorageExtension>();

	Goose db(nullptr, &config);
	Connection con(db);

	// this works since it is the full name
	auto query = string("ATTACH ':memory:' AS db1 (TYPE SQLITE_SCANNER)");
	auto result = con.Query(query);
	if (result->HasError()) {
		FAIL("Query failed even though sqlite_scanner is registered."
		     "Query: " +
		     query + "\n" + "Error: " + result->GetError());
	}
}

TEST_CASE("Test storage extension lookup alias", "[api]") {
	DBConfig config;

	// Register a storage extension under its CANONICAL name "sqlite_scanner"
	// This mimics how the real sqlite_scanner extension registers itself
	// there is a hard-coded alias "sqlite" for "sqlite_scanner"
	config.storage_extensions["sqlite_scanner"] = make_uniq<DummyStorageExtension>();

	Goose db(nullptr, &config);
	Connection con(db);

	// Without ApplyExtensionAlias in database_manager.cc,
	// this fails with an error about not finding the extension
	auto query = string("ATTACH ':memory:' AS db1 (TYPE SQLITE)");
	auto result = con.Query(query);
	if (result->HasError()) {
		FAIL("Query failed even though sqlite_scanner is registered.\n"
		     "Query: " +
		     query + "\n" + "Error: " + result->GetError());
	}
}
