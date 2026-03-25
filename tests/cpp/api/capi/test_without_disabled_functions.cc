#include <goose/testing/catch.h>
#define GOOSE_API_NO_DEPRECATED

#include <goose/goose-c.h>

using namespace std;

// we only use functions that are cool to use in the 1.0 API
TEST_CASE("Test without deprecated or future moved functions", "[capi]") {
	goose_database database;
	goose_connection connection;
	goose_prepared_statement statement;
	goose_result result;

	REQUIRE(goose_open(NULL, &database) == GooseSuccess);
	REQUIRE(goose_connect(database, &connection) == GooseSuccess);
	REQUIRE(goose_prepare(connection, "SELECT ?::INTEGER AS a", &statement) == GooseSuccess);
	REQUIRE(goose_bind_int32(statement, 1, 42) == GooseSuccess);
	REQUIRE(goose_execute_prepared(statement, &result) == GooseSuccess);

	REQUIRE(goose_column_count(&result) == 1);
	REQUIRE(string(goose_column_name(&result, 0)) == "a");
	REQUIRE(goose_column_type(&result, 0) == GOOSE_TYPE_INTEGER);

	auto chunk = goose_fetch_chunk(result);
	REQUIRE(chunk);
	auto vector = goose_data_chunk_get_vector(chunk, 0);
	REQUIRE(vector);
	auto validity = goose_vector_get_validity(vector);
	REQUIRE(goose_validity_row_is_valid(validity, 0));
	auto data = (int *)goose_vector_get_data(vector);
	REQUIRE(data);
	REQUIRE(data[0] == 42);

	goose_destroy_data_chunk(&chunk);
	goose_destroy_result(&result);
	goose_destroy_prepare(&statement);
	goose_disconnect(&connection);
	goose_close(&database);
}
