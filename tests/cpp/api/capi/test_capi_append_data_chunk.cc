#include <goose/testing/capi_tester.h>

using namespace goose;
using namespace std;

TEST_CASE("Test casting columns in AppendDataChunk in C API", "[capi]") {
	goose::vector<string> tables;
	tables.push_back("CREATE TABLE test(i BIGINT, j VARCHAR);");
	tables.push_back("CREATE TABLE test(i BIGINT, j BOOLEAN);");

	for (idx_t i = 0; i < tables.size(); i++) {
		CAPITester tester;
		REQUIRE(tester.OpenDatabase(nullptr));
		REQUIRE(goose_vector_size() == STANDARD_VECTOR_SIZE);

		tester.Query(tables[i]);

		goose_logical_type types[2];
		types[0] = goose_create_logical_type(GOOSE_TYPE_SMALLINT);
		types[1] = goose_create_logical_type(GOOSE_TYPE_BOOLEAN);

		auto data_chunk = goose_create_data_chunk(types, 2);
		REQUIRE(data_chunk);

		auto smallint_col = goose_data_chunk_get_vector(data_chunk, 0);
		auto boolean_col = goose_data_chunk_get_vector(data_chunk, 1);

		auto smallint_data = reinterpret_cast<int16_t *>(goose_vector_get_data(smallint_col));
		smallint_data[0] = 15;
		smallint_data[1] = -15;

		auto boolean_data = reinterpret_cast<bool *>(goose_vector_get_data(boolean_col));
		boolean_data[0] = false;
		boolean_data[1] = true;

		goose_data_chunk_set_size(data_chunk, 2);

		goose_appender appender;
		auto status = goose_appender_create(tester.connection, nullptr, "test", &appender);
		REQUIRE(status == GooseSuccess);

		REQUIRE(goose_append_data_chunk(appender, data_chunk) == GooseSuccess);
		goose_appender_close(appender);

		auto result = tester.Query("SELECT i, j FROM test;");
		REQUIRE(result->Fetch<int64_t>(0, 0) == 15);
		REQUIRE(result->Fetch<int64_t>(0, 1) == -15);
		auto str = result->Fetch<string>(1, 0);
		REQUIRE(str.compare("false") == 0);
		str = result->Fetch<string>(1, 1);
		REQUIRE(str.compare("true") == 0);

		goose_appender_destroy(&appender);
		goose_destroy_data_chunk(&data_chunk);
		goose_destroy_logical_type(&types[0]);
		goose_destroy_logical_type(&types[1]);
	}
}

TEST_CASE("Test casting error in AppendDataChunk in C API", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));
	REQUIRE(goose_vector_size() == STANDARD_VECTOR_SIZE);

	tester.Query("CREATE TABLE test(i BIGINT, j BOOLEAN[]);");

	goose_logical_type types[2];
	types[0] = goose_create_logical_type(GOOSE_TYPE_SMALLINT);
	types[1] = goose_create_logical_type(GOOSE_TYPE_BOOLEAN);

	auto data_chunk = goose_create_data_chunk(types, 2);
	REQUIRE(data_chunk);

	auto smallint_col = goose_data_chunk_get_vector(data_chunk, 0);
	auto boolean_col = goose_data_chunk_get_vector(data_chunk, 1);

	auto smallint_data = reinterpret_cast<int16_t *>(goose_vector_get_data(smallint_col));
	smallint_data[0] = 15;
	smallint_data[1] = -15;

	auto boolean_data = reinterpret_cast<bool *>(goose_vector_get_data(boolean_col));
	boolean_data[0] = false;
	boolean_data[1] = true;

	goose_data_chunk_set_size(data_chunk, 2);

	goose_appender appender;
	auto status = goose_appender_create(tester.connection, nullptr, "test", &appender);
	REQUIRE(status == GooseSuccess);

	REQUIRE(goose_append_data_chunk(appender, data_chunk) == GooseError);
	auto error_msg = goose_appender_error(appender);
	REQUIRE(string(error_msg) == "type mismatch in AppendDataChunk, expected BOOLEAN[], got BOOLEAN for column 1");

	goose_appender_close(appender);
	goose_appender_destroy(&appender);
	goose_destroy_data_chunk(&data_chunk);
	goose_destroy_logical_type(&types[0]);
	goose_destroy_logical_type(&types[1]);
}

TEST_CASE("Test casting timestamps in AppendDataChunk in C API", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));
	REQUIRE(goose_vector_size() == STANDARD_VECTOR_SIZE);

	tester.Query("CREATE TABLE test(i TIMESTAMP, j DATE);");

	goose_logical_type types[2];
	types[0] = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	types[1] = goose_create_logical_type(GOOSE_TYPE_VARCHAR);

	auto data_chunk = goose_create_data_chunk(types, 2);
	REQUIRE(data_chunk);

	auto ts_column = goose_data_chunk_get_vector(data_chunk, 0);
	auto date_column = goose_data_chunk_get_vector(data_chunk, 1);

	goose_vector_assign_string_element(ts_column, 0, "2017-07-23 13:10:11");
	goose_vector_assign_string_element(date_column, 0, "1993-08-14");
	goose_data_chunk_set_size(data_chunk, 1);

	goose_appender appender;
	auto status = goose_appender_create(tester.connection, nullptr, "test", &appender);
	REQUIRE(status == GooseSuccess);

	REQUIRE(goose_append_data_chunk(appender, data_chunk) == GooseSuccess);
	goose_appender_close(appender);

	auto result = tester.Query("SELECT i::VARCHAR, j::VARCHAR FROM test;");
	auto str = result->Fetch<string>(0, 0);
	REQUIRE(str.compare("2017-07-23 13:10:11") == 0);
	str = result->Fetch<string>(1, 0);
	REQUIRE(str.compare("1993-08-14") == 0);

	goose_appender_destroy(&appender);
	goose_destroy_data_chunk(&data_chunk);
	goose_destroy_logical_type(&types[0]);
	goose_destroy_logical_type(&types[1]);
}
