#include "capi_tester.h"
#include <goose/goose-c.h>

#include <cstring> // for strcmp

using namespace goose;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------
// Test Catalog Interface in C API
//----------------------------------------------------------------------------------------------------------------------

static void Execute(goose_function_info info, goose_data_chunk input, goose_vector output) {
	auto conn = (goose_connection)goose_scalar_function_get_extra_info(info);

	goose_client_context context;
	goose_connection_get_client_context(conn, &context);

	// Get first argument (schema name)
	auto input_size = goose_data_chunk_get_size(input);

	auto catalog_vector = goose_data_chunk_get_vector(input, 0);
	auto catalog_data = (goose_string_t *)goose_vector_get_data(catalog_vector);

	auto schema_vector = goose_data_chunk_get_vector(input, 1);
	auto schema_data = (goose_string_t *)goose_vector_get_data(schema_vector);

	auto name_vector = goose_data_chunk_get_vector(input, 2);
	auto name_data = (goose_string_t *)goose_vector_get_data(name_vector);

	auto result_data = (bool *)goose_vector_get_data(output);

	for (idx_t row_idx = 0; row_idx < input_size; row_idx++) {
		auto catalog_str = catalog_data[row_idx];
		string catalog_name(goose_string_t_data(&catalog_str), goose_string_t_length(catalog_str));

		goose_catalog catalog = goose_client_context_get_catalog(context, catalog_name.c_str());
		if (catalog == nullptr) {
			result_data[row_idx] = false;
			continue;
		}

		// Verify that this is a goose catalog
		auto catalog_type = goose_catalog_get_type_name(catalog);
		if (strcmp(catalog_type, "goose") != 0) {
			goose_destroy_catalog(&catalog);
			goose_destroy_client_context(&context);
			goose_scalar_function_set_error(info, "Catalog type is not goose");
			return;
		}

		auto schema_str = schema_data[row_idx];
		auto name_str = name_data[row_idx];

		string schema_name(goose_string_t_data(&schema_str), goose_string_t_length(schema_str));
		string name(goose_string_t_data(&name_str), goose_string_t_length(name_str));

		auto entry = goose_catalog_get_entry(catalog, context, GOOSE_CATALOG_ENTRY_TYPE_TABLE, schema_name.c_str(),
		                                      name.c_str());

		// Check result
		result_data[row_idx] = (entry != nullptr);

		if (entry) {
			// Verify that we actually got what we expected
			auto entry_type = goose_catalog_entry_get_type(entry);
			if (entry_type != GOOSE_CATALOG_ENTRY_TYPE_TABLE) {
				goose_destroy_catalog_entry(&entry);
				goose_destroy_catalog(&catalog);
				goose_destroy_client_context(&context);
				goose_scalar_function_set_error(info, "Catalog entry type is not TABLE");
				return;
			}

			auto entry_name = goose_catalog_entry_get_name(entry);
			if (strcmp(entry_name, name.c_str()) != 0) {
				goose_destroy_catalog_entry(&entry);
				goose_destroy_catalog(&catalog);
				goose_destroy_client_context(&context);
				goose_scalar_function_set_error(info, "Catalog entry name does not match");
				return;
			}
		}

		goose_destroy_catalog_entry(&entry);
		goose_destroy_catalog_entry(&entry);

		goose_destroy_catalog(&catalog);
		goose_destroy_catalog(&catalog);
	}

	goose_destroy_client_context(&context);
}

TEST_CASE("Test Catalog Interface in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));

	// Get the client context to pass as extra info
	goose_client_context context;
	goose_connection_get_client_context(tester.connection, &context);

	// We cant access the catalog outside of a transaction
	auto catalog = goose_client_context_get_catalog(context, "goose");
	REQUIRE(catalog == nullptr);

	goose_destroy_client_context(&context);
	goose_destroy_catalog(&catalog);

	// We need a scalar function to get the catalog (from within a transaction)
	auto func = goose_create_scalar_function();
	goose_scalar_function_set_name(func, "test_lookup_function");
	auto arg_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto ret_type = goose_create_logical_type(GOOSE_TYPE_BOOLEAN);
	goose_scalar_function_add_parameter(func, arg_type);
	goose_scalar_function_add_parameter(func, arg_type);
	goose_scalar_function_add_parameter(func, arg_type);
	goose_scalar_function_set_return_type(func, ret_type);
	goose_scalar_function_set_function(func, Execute);
	goose_scalar_function_set_extra_info(func, tester.connection, nullptr);

	REQUIRE(goose_register_scalar_function(tester.connection, func) == GooseSuccess);
	goose_destroy_scalar_function(&func);
	goose_destroy_logical_type(&ret_type);
	goose_destroy_logical_type(&arg_type);

	// Execute a query to test with non-existing catalog
	result = tester.Query("SELECT test_lookup_function('non_existing_catalog', 'main', 'test_table') AS exists;");
	REQUIRE_NO_FAIL(*result);

	// Execute a query to test the catalog interface with a non-existing schema
	result = tester.Query("SELECT test_lookup_function('memory', 'foobar', 'test_table') AS exists;");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<bool>(0, 0) == false);

	// Execute a query to test the catalog interface with a non-existing table
	result = tester.Query("SELECT test_lookup_function('memory', 'main', 'test_table') AS exists;");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<bool>(0, 0) == false);

	// Now create a table and test again
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE test_table(i INTEGER);"));

	result = tester.Query("SELECT test_lookup_function('memory', 'main', 'test_table') AS exists;");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<bool>(0, 0) == true);
}
