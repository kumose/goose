#include <goose/testing/capi_tester.h>
#include <goose/goose-c.h>

using namespace goose;
using namespace std;

TEST_CASE("Test the table description in the C API", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));
	goose_table_description table_description = nullptr;
	tester.Query("SET threads=1;");

	// Test a non-existent table.
	auto status = goose_table_description_create(tester.connection, nullptr, "test", &table_description);
	REQUIRE(status == GooseError);
	goose_table_description_destroy(&table_description);

	status = goose_table_description_create_ext(tester.connection, "hello", "world", "test", &table_description);
	REQUIRE(status == GooseError);
	goose_table_description_destroy(&table_description);

	// Create an in-memory table and a table in an external file.
	tester.Query("CREATE TABLE test (i INTEGER, j INTEGER default 5)");
	auto test_dir = TestDirectoryPath();
	auto attach_query = "ATTACH '" + test_dir + "/ext_description.db'";
	tester.Query(attach_query);
	tester.Query("CREATE TABLE ext_description.test(my_column INTEGER)");

	// Test invalid catalog and schema.
	status =
	    goose_table_description_create_ext(tester.connection, "non-existent", nullptr, "test", &table_description);
	REQUIRE(status == GooseError);
	goose_table_description_destroy(&table_description);

	status = goose_table_description_create(tester.connection, "non-existent", "test", &table_description);
	REQUIRE(status == GooseError);
	goose_table_description_destroy(&table_description);

	status = goose_table_description_create(tester.connection, nullptr, "test", &table_description);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_table_description_error(table_description) == nullptr);

	bool has_default;
	SECTION("Passing nullptr to has_default") {
		REQUIRE(goose_column_has_default(table_description, 2, nullptr) == GooseError);
		REQUIRE(goose_column_has_default(nullptr, 2, &has_default) == GooseError);
	}
	SECTION("Out of range column for has_default") {
		REQUIRE(goose_column_has_default(table_description, 2, &has_default) == GooseError);
	}
	SECTION("In range column - not default") {
		REQUIRE(goose_column_has_default(table_description, 0, &has_default) == GooseSuccess);
		REQUIRE(has_default == false);
	}
	SECTION("In range column - default") {
		REQUIRE(goose_column_has_default(table_description, 1, &has_default) == GooseSuccess);
		REQUIRE(has_default == true);
	}
	goose_table_description_destroy(&table_description);

	// Let's get information about the external table.
	status =
	    goose_table_description_create_ext(tester.connection, "ext_description", nullptr, "test", &table_description);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_table_description_error(table_description) == nullptr);

	SECTION("Passing nullptr to get_column_count") {
		REQUIRE(goose_table_description_get_column_count(nullptr) == 0);
	}
	SECTION("Passing nullptr to get_name") {
		REQUIRE(goose_table_description_get_column_name(nullptr, 0) == nullptr);
	}
	SECTION("Passing nullptr to get_type") {
		REQUIRE(goose_table_description_get_column_type(nullptr, 0) == nullptr);
	}
	SECTION("Out of range column for get_name") {
		REQUIRE(goose_table_description_get_column_name(table_description, 1) == nullptr);
	}
	SECTION("Out of range column for get_type") {
		REQUIRE(goose_table_description_get_column_type(table_description, 1) == nullptr);
	}
	SECTION("get the column count") {
		auto column_count = goose_table_description_get_column_count(table_description);
		REQUIRE(column_count == 1);
	}
	SECTION("In range column - get the name") {
		auto column_name = goose_table_description_get_column_name(table_description, 0);
		string expected = "my_column";
		REQUIRE(!expected.compare(column_name));
		goose_free(column_name);
	}
	SECTION("In range column - get the type") {
		auto column_type = goose_table_description_get_column_type(table_description, 0);
		auto type_id = goose_get_type_id(column_type);
		REQUIRE(type_id == GOOSE_TYPE_INTEGER);
		goose_destroy_logical_type(&column_type);
	}
	goose_table_description_destroy(&table_description);
}

TEST_CASE("Test getting the table names of a query in the C API", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	tester.Query("CREATE SCHEMA schema1");
	tester.Query("CREATE SCHEMA \"schema.2\"");
	tester.Query("CREATE TABLE schema1.\"table.1\"(i INT)");
	tester.Query("CREATE TABLE \"schema.2\".\"table.2\"(i INT)");

	string query = "SELECT * FROM schema1.\"table.1\", \"schema.2\".\"table.2\"";
	auto table_name_values = goose_get_table_names(tester.connection, query.c_str(), true);

	auto size = goose_get_list_size(table_name_values);
	REQUIRE(size == 2);

	goose::unordered_set<string> expected_names = {"schema1.\"table.1\"", "\"schema.2\".\"table.2\""};
	for (idx_t i = 0; i < size; i++) {
		auto name_value = goose_get_list_child(table_name_values, i);
		auto name = goose_get_varchar(name_value);
		REQUIRE(expected_names.count(name) == 1);
		goose_free(name);
		goose_destroy_value(&name_value);
	}

	goose_destroy_value(&table_name_values);
	tester.Cleanup();
}
