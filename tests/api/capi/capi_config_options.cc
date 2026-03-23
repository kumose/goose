#include "capi_tester.h"

using namespace goose;
using namespace std;

TEST_CASE("Test Custom Configuration Options in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	// Create a connection and get client context
	REQUIRE(tester.OpenDatabase(nullptr));
	goose_client_context context;
	goose_connection_get_client_context(tester.connection, &context);
	REQUIRE(context != nullptr);

	// Setup some types for config options
	auto str_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);

	auto default_str_value = goose_create_varchar("default_string");
	auto default_int_value = goose_create_int32(42);

	// Test 1: Create a string config option
	auto str_opt = goose_create_config_option();
	REQUIRE(str_opt != nullptr);
	goose_config_option_set_name(str_opt, "test_string_option");
	goose_config_option_set_type(str_opt, str_type);
	goose_config_option_set_description(str_opt, "A test string configuration option");
	goose_config_option_set_default_value(str_opt, default_str_value);
	REQUIRE(goose_register_config_option(tester.connection, str_opt) == GooseSuccess);

	// Get and verify the string option
	result = tester.Query("SELECT current_setting('test_string_option')");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<string>(0, 0) == "default_string");

	// Set and verify the string option
	REQUIRE_NO_FAIL(tester.Query("SET test_string_option = 'new_value'"));
	result = tester.Query("SELECT current_setting('test_string_option')");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<string>(0, 0) == "new_value");

	// Also get it from the client context
	goose_config_option_scope scope;
	auto retrieved_str = goose_client_context_get_config_option(context, "test_string_option", &scope);
	REQUIRE(retrieved_str != nullptr);
	goose_destroy_value(&retrieved_str);

	// By default, the scope should be SESSION
	REQUIRE(scope == GOOSE_CONFIG_OPTION_SCOPE_SESSION);

	goose_destroy_config_option(&str_opt);
	goose_destroy_config_option(&str_opt);

	// Fetch it from the goose_settings() function
	result =
	    tester.Query("SELECT name, value, input_type, scope FROM goose_settings() WHERE name = 'test_string_option'");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ChunkCount() == 1);
	REQUIRE(result->Fetch<string>(0, 0) == "test_string_option");
	REQUIRE(result->Fetch<string>(1, 0) == "new_value");
	REQUIRE(result->Fetch<string>(2, 0) == "VARCHAR");
	REQUIRE(result->Fetch<string>(3, 0) == "LOCAL");

	// Test 2: Create an integer config option, with the type inferred from the default value
	auto int_opt = goose_create_config_option();
	REQUIRE(int_opt != nullptr);
	goose_config_option_set_name(int_opt, "test_integer_option");
	goose_config_option_set_description(int_opt, "A test integer configuration option");
	goose_config_option_set_default_value(int_opt, default_int_value);
	goose_config_option_set_default_scope(int_opt, GOOSE_CONFIG_OPTION_SCOPE_GLOBAL);
	REQUIRE(goose_register_config_option(tester.connection, int_opt) == GooseSuccess);

	// Get and verify the integer option
	result = tester.Query("SELECT current_setting('test_integer_option')");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<int32_t>(0, 0) == 42);

	// Also get it from the client context. Try without specifying scope first
	auto retrieved_int = goose_client_context_get_config_option(context, "test_integer_option", nullptr);
	REQUIRE(retrieved_int != nullptr);
	REQUIRE(goose_get_int32(retrieved_int) == 42);
	goose_destroy_value(&retrieved_int);

	// By default, the scope should be GLOBAL, as that was what we defined this option with
	retrieved_int = goose_client_context_get_config_option(context, "test_integer_option", &scope);
	REQUIRE(retrieved_int != nullptr);
	goose_destroy_value(&retrieved_int);

	// Also check in goose_settings()
	result =
	    tester.Query("SELECT name, value, input_type, scope FROM goose_settings() WHERE name = 'test_integer_option'");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ChunkCount() == 1);
	REQUIRE(result->Fetch<string>(0, 0) == "test_integer_option");
	REQUIRE(result->Fetch<string>(1, 0) == "42");
	REQUIRE(result->Fetch<string>(2, 0) == "INTEGER");
	REQUIRE(result->Fetch<string>(3, 0) == "GLOBAL");

	goose_destroy_config_option(&int_opt);
	goose_destroy_config_option(&int_opt);

	// Cleanup
	goose_destroy_client_context(&context);
	goose_destroy_logical_type(&str_type);
	goose_destroy_logical_type(&int_type);

	goose_destroy_value(&default_str_value);
	goose_destroy_value(&default_int_value);
}
