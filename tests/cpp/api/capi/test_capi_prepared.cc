#include <goose/testing/capi_tester.h>

using namespace goose;
using namespace std;

TEST_CASE("Test prepared statements in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	goose_result res;
	goose_prepared_statement stmt = nullptr;
	goose_state status;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	status = goose_prepare(tester.connection, "SELECT CAST($1 AS BIGINT)", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);

	REQUIRE(goose_prepared_statement_column_count(stmt) == 1);
	REQUIRE(goose_prepared_statement_column_type(stmt, 0) == GOOSE_TYPE_BIGINT);
	auto logical_type = goose_prepared_statement_column_logical_type(stmt, 0);
	REQUIRE(logical_type);
	REQUIRE(goose_get_type_id(logical_type) == GOOSE_TYPE_BIGINT);
	goose_destroy_logical_type(&logical_type);

	status = goose_bind_boolean(stmt, 1, true);
	REQUIRE(status == GooseSuccess);

	// Parameter index 2 is out of bounds
	status = goose_bind_boolean(stmt, 2, true);
	REQUIRE(status == GooseError);

	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 1);
	goose_destroy_result(&res);

	goose_bind_int8(stmt, 1, 8);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 8);
	goose_destroy_result(&res);

	goose_bind_int16(stmt, 1, 16);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 16);
	goose_destroy_result(&res);

	goose_bind_int32(stmt, 1, 32);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 32);
	goose_destroy_result(&res);

	goose_bind_int64(stmt, 1, 64);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 64);
	goose_destroy_result(&res);

	goose_bind_hugeint(stmt, 1, goose_double_to_hugeint(64));
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_hugeint_to_double(goose_value_hugeint(&res, 0, 0)) == 64.0);
	goose_destroy_result(&res);

	goose_bind_uhugeint(stmt, 1, goose_double_to_uhugeint(64));
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_uhugeint_to_double(goose_value_uhugeint(&res, 0, 0)) == 64.0);
	goose_destroy_result(&res);

	// Fetching a DECIMAL from a non-DECIMAL result returns 0
	goose_decimal decimal = goose_double_to_decimal(634.3453, 7, 4);
	goose_bind_decimal(stmt, 1, decimal);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	goose_decimal result_decimal = goose_value_decimal(&res, 0, 0);
	REQUIRE(result_decimal.scale == 0);
	REQUIRE(result_decimal.width == 0);
	REQUIRE(result_decimal.value.upper == 0);
	REQUIRE(result_decimal.value.lower == 0);
	goose_destroy_result(&res);

	goose_bind_uint8(stmt, 1, 8);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_uint8(&res, 0, 0) == 8);
	goose_destroy_result(&res);

	goose_bind_uint16(stmt, 1, 8);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_uint16(&res, 0, 0) == 8);
	goose_destroy_result(&res);

	goose_bind_uint32(stmt, 1, 8);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_uint32(&res, 0, 0) == 8);
	goose_destroy_result(&res);

	goose_bind_uint64(stmt, 1, 8);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_uint64(&res, 0, 0) == 8);
	goose_destroy_result(&res);

	goose_bind_float(stmt, 1, 42.0);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 42);
	goose_destroy_result(&res);

	goose_bind_double(stmt, 1, 43.0);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 43);
	goose_destroy_result(&res);

	REQUIRE(goose_bind_float(stmt, 1, NAN) == GooseSuccess);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseError);
	goose_destroy_result(&res);

	REQUIRE(goose_bind_double(stmt, 1, NAN) == GooseSuccess);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseError);
	goose_destroy_result(&res);

	REQUIRE(goose_bind_varchar(stmt, 1, "\x80\x40\x41") == GooseError);
	goose_bind_varchar(stmt, 1, "44");
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 44);
	goose_destroy_result(&res);

	goose_bind_null(stmt, 1);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_nullmask_data(&res, 0)[0] == true);
	goose_destroy_result(&res);

	goose_destroy_prepare(&stmt);
	// again to make sure it does not crash
	goose_destroy_result(&res);
	goose_destroy_prepare(&stmt);

	status = goose_prepare(tester.connection, "SELECT CAST($1 AS VARCHAR)", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);

	// invalid unicode
	REQUIRE(goose_bind_varchar_length(stmt, 1, "\x80", 1) == GooseError);
	// we can bind null values, though!
	REQUIRE(goose_bind_varchar_length(stmt, 1, "\x00\x40\x41", 3) == GooseSuccess);
	goose_bind_varchar_length(stmt, 1, "hello world", 5);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	auto value = goose_value_varchar(&res, 0, 0);
	REQUIRE(string(value) == "hello");
	REQUIRE(goose_value_int8(&res, 0, 0) == 0);
	goose_free(value);
	goose_destroy_result(&res);

	goose_bind_blob(stmt, 1, "hello\0world", 11);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	value = goose_value_varchar(&res, 0, 0);
	REQUIRE(string(value) == "hello\\x00world");
	REQUIRE(goose_value_int8(&res, 0, 0) == 0);
	goose_free(value);
	goose_destroy_result(&res);

	goose_date_struct date_struct;
	date_struct.year = 1992;
	date_struct.month = 9;
	date_struct.day = 3;

	goose_bind_date(stmt, 1, goose_to_date(date_struct));
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	value = goose_value_varchar(&res, 0, 0);
	REQUIRE(string(value) == "1992-09-03");
	goose_free(value);
	goose_destroy_result(&res);

	goose_time_struct time_struct;
	time_struct.hour = 12;
	time_struct.min = 22;
	time_struct.sec = 33;
	time_struct.micros = 123400;

	goose_bind_time(stmt, 1, goose_to_time(time_struct));
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	value = goose_value_varchar(&res, 0, 0);
	REQUIRE(string(value) == "12:22:33.1234");
	goose_free(value);
	goose_destroy_result(&res);

	goose_timestamp_struct ts;
	ts.date = date_struct;
	ts.time = time_struct;

	goose_bind_timestamp(stmt, 1, goose_to_timestamp(ts));
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	value = goose_value_varchar(&res, 0, 0);
	REQUIRE(string(value) == "1992-09-03 12:22:33.1234");
	goose_free(value);
	goose_destroy_result(&res);

	goose_bind_timestamp_tz(stmt, 1, goose_to_timestamp(ts));
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	value = goose_value_varchar(&res, 0, 0);
	REQUIRE(StringUtil::Contains(string(value), "1992-09"));
	goose_free(value);
	goose_destroy_result(&res);

	goose_interval interval;
	interval.months = 3;
	interval.days = 0;
	interval.micros = 0;

	goose_bind_interval(stmt, 1, interval);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	value = goose_value_varchar(&res, 0, 0);
	REQUIRE(string(value) == "3 months");
	goose_free(value);
	goose_destroy_result(&res);

	goose_destroy_prepare(&stmt);

	status = goose_query(tester.connection, "CREATE TABLE a (i INTEGER)", NULL);
	REQUIRE(status == GooseSuccess);

	status = goose_prepare(tester.connection, "INSERT INTO a VALUES (?)", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);
	REQUIRE(goose_nparams(nullptr) == 0);
	REQUIRE(goose_nparams(stmt) == 1);
	REQUIRE(goose_param_type(nullptr, 0) == GOOSE_TYPE_INVALID);
	REQUIRE(goose_param_type(stmt, 0) == GOOSE_TYPE_INVALID);
	REQUIRE(goose_param_type(stmt, 1) == GOOSE_TYPE_INTEGER);
	REQUIRE(goose_param_type(stmt, 2) == GOOSE_TYPE_INVALID);

	for (int32_t i = 1; i <= 1000; i++) {
		goose_bind_int32(stmt, 1, i);
		status = goose_execute_prepared(stmt, nullptr);
		REQUIRE(status == GooseSuccess);
	}
	goose_destroy_prepare(&stmt);

	status = goose_prepare(tester.connection, "SELECT SUM(i)*$1-$2 FROM a", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);
	// clear bindings
	goose_bind_int32(stmt, 1, 2);
	REQUIRE(goose_clear_bindings(stmt) == GooseSuccess);

	// bind again will succeed
	goose_bind_int32(stmt, 1, 2);
	goose_bind_int32(stmt, 2, 1000);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int32(&res, 0, 0) == 1000000);
	goose_destroy_result(&res);
	goose_destroy_prepare(&stmt);

	// not-so-happy path
	status = goose_prepare(tester.connection, "SELECT XXXXX", &stmt);
	REQUIRE(status == GooseError);
	goose_destroy_prepare(&stmt);

	status = goose_prepare(tester.connection, "SELECT CAST($1 AS INTEGER)", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);

	REQUIRE(goose_prepared_statement_column_count(stmt) == 1);
	REQUIRE(goose_prepared_statement_column_type(stmt, 0) == GOOSE_TYPE_INTEGER);

	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseError);
	goose_destroy_result(&res);
	goose_destroy_prepare(&stmt);

	// test goose_malloc explicitly
	auto malloced_data = goose_malloc(100);
	memcpy(malloced_data, "hello\0", 6);
	REQUIRE(string((char *)malloced_data) == "hello");
	goose_free(malloced_data);

	status = goose_prepare(tester.connection, "SELECT sum(i) FROM a WHERE i > ?", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);
	REQUIRE(goose_nparams(stmt) == 1);
	REQUIRE(goose_param_type(nullptr, 0) == GOOSE_TYPE_INVALID);
	REQUIRE(goose_param_type(stmt, 1) == GOOSE_TYPE_INTEGER);

	REQUIRE(goose_prepared_statement_column_count(stmt) == 1);
	REQUIRE(goose_prepared_statement_column_type(stmt, 0) == GOOSE_TYPE_HUGEINT);

	goose_destroy_prepare(&stmt);
}

TEST_CASE("Test goose_prepared_statement return value APIs", "[capi]") {
	goose_database db;
	goose_connection conn;
	goose_prepared_statement stmt;
	REQUIRE(goose_open("", &db) == GooseSuccess);
	REQUIRE(goose_connect(db, &conn) == GooseSuccess);

	// Unambiguous return column types
	REQUIRE(goose_prepare(conn, "select $1::TEXT, $2::integer, $3::BOOLEAN, $4::FLOAT, $5::DOUBLE", &stmt) ==
	        GooseSuccess);

	REQUIRE(goose_prepared_statement_column_count(stmt) == 5);
	auto expected_types = {GOOSE_TYPE_VARCHAR, GOOSE_TYPE_INTEGER, GOOSE_TYPE_BOOLEAN, GOOSE_TYPE_FLOAT,
	                       GOOSE_TYPE_DOUBLE};

	for (idx_t i = 0; i < 5; i++) {
		REQUIRE(goose_prepared_statement_column_type(stmt, i) == *next(expected_types.begin(), i));
		auto logical_type = goose_prepared_statement_column_logical_type(stmt, i);
		REQUIRE(logical_type);
		REQUIRE(goose_get_type_id(logical_type) == *next(expected_types.begin(), i));
		goose_destroy_logical_type(&logical_type);
	}

	auto column_name = goose_prepared_statement_column_name(stmt, 0);
	std::string col_name_str = column_name;
	goose_free((void *)column_name);
	REQUIRE(col_name_str == "CAST($1 AS VARCHAR)");

	goose_destroy_prepare(&stmt);

	// Return columns contain ambiguous types
	REQUIRE(goose_prepare(conn, "select $1::TEXT, $2::integer, $3, $4::BOOLEAN, $5::FLOAT, $6::DOUBLE", &stmt) ==
	        GooseSuccess);

	REQUIRE(goose_prepared_statement_column_count(stmt) == 1);
	REQUIRE(goose_prepared_statement_column_type(stmt, 0) == GOOSE_TYPE_INVALID);

	auto logical_type = goose_prepared_statement_column_logical_type(stmt, 0);
	REQUIRE(logical_type);
	REQUIRE(goose_get_type_id(logical_type) == GOOSE_TYPE_INVALID);
	goose_destroy_logical_type(&logical_type);

	auto col_name_ptr = goose_prepared_statement_column_name(stmt, 0);
	col_name_str = col_name_ptr;
	goose_free((void *)col_name_ptr);
	REQUIRE(col_name_str == "unknown");
	REQUIRE(goose_prepared_statement_column_name(stmt, 1) == nullptr);
	REQUIRE(goose_prepared_statement_column_name(stmt, 5) == nullptr);

	goose_destroy_prepare(&stmt);
	goose_disconnect(&conn);
	goose_close(&db);
}

TEST_CASE("Test goose_param_type and goose_param_logical_type", "[capi]") {
	goose_database db;
	goose_connection conn;
	goose_prepared_statement stmt;

	REQUIRE(goose_open("", &db) == GooseSuccess);
	REQUIRE(goose_connect(db, &conn) == GooseSuccess);
	REQUIRE(goose_prepare(conn, "select $1::integer, $2::integer", &stmt) == GooseSuccess);

	auto logical_type = goose_param_logical_type(stmt, 2);
	REQUIRE(logical_type);
	REQUIRE(goose_get_type_id(logical_type) == GOOSE_TYPE_INTEGER);
	goose_destroy_logical_type(&logical_type);

	REQUIRE(goose_param_type(stmt, 2) == GOOSE_TYPE_INTEGER);
	REQUIRE(goose_bind_null(stmt, 1) == GooseSuccess);
	REQUIRE(goose_bind_int32(stmt, 2, 10) == GooseSuccess);

	REQUIRE(!goose_param_logical_type(nullptr, 2));
	REQUIRE(goose_param_type(nullptr, 2) == GOOSE_TYPE_INVALID);
	REQUIRE(!goose_param_logical_type(stmt, 2000));
	REQUIRE(goose_param_type(stmt, 2000) == GOOSE_TYPE_INVALID);

	goose_result result;
	REQUIRE(goose_execute_prepared(stmt, &result) == GooseSuccess);
	REQUIRE(goose_param_type(stmt, 2) == GOOSE_TYPE_INTEGER);
	goose_clear_bindings(stmt);
	goose_destroy_result(&result);

	goose_destroy_prepare(&stmt);
	goose_disconnect(&conn);
	goose_close(&db);
}

TEST_CASE("Test prepared statements with named parameters in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	goose_result res;
	goose_prepared_statement stmt = nullptr;
	goose_state status;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	status = goose_prepare(tester.connection, "SELECT CAST($my_val AS BIGINT)", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);

	idx_t parameter_index;
	// test invalid name
	status = goose_bind_parameter_index(stmt, &parameter_index, "invalid");
	REQUIRE(status == GooseError);

	status = goose_bind_parameter_index(stmt, &parameter_index, "my_val");
	REQUIRE(status == GooseSuccess);

	REQUIRE(goose_param_type(stmt, 1) == GOOSE_TYPE_BIGINT);

	auto logical_type = goose_param_logical_type(stmt, 1);
	REQUIRE(logical_type);
	REQUIRE(goose_get_type_id(logical_type) == GOOSE_TYPE_BIGINT);
	goose_destroy_logical_type(&logical_type);

	idx_t param_count = goose_nparams(stmt);
	goose::vector<string> names;
	for (idx_t i = 0; i < param_count; i++) {
		auto name = goose_parameter_name(stmt, i + 1);
		names.push_back(std::string(name));
		goose_free((void *)name);
	}

	REQUIRE(goose_parameter_name(stmt, 0) == (const char *)NULL);
	REQUIRE(goose_parameter_name(stmt, 2) == (const char *)NULL);

	REQUIRE(goose_prepared_statement_column_count(stmt) == 1);
	REQUIRE(goose_prepared_statement_column_type(stmt, 0) == GOOSE_TYPE_BIGINT);

	goose::vector<string> expected_names = {"my_val"};
	REQUIRE(names.size() == expected_names.size());
	for (idx_t i = 0; i < expected_names.size(); i++) {
		auto &name = names[i];
		auto &expected_name = expected_names[i];
		REQUIRE(name == expected_name);
	}

	status = goose_bind_boolean(stmt, parameter_index, 1);
	REQUIRE(status == GooseSuccess);
	status = goose_bind_boolean(stmt, parameter_index + 1, 1);
	REQUIRE(status == GooseError);

	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 1);
	goose_destroy_result(&res);

	// Clear the bindings, don't rebind the parameter index
	status = goose_clear_bindings(stmt);
	REQUIRE(status == GooseSuccess);

	status = goose_bind_boolean(stmt, parameter_index, 1);
	REQUIRE(status == GooseSuccess);

	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 1);
	goose_destroy_result(&res);

	goose_destroy_prepare(&stmt);
}

TEST_CASE("Maintain prepared statement types", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	goose_result res;
	goose_prepared_statement stmt = nullptr;
	goose_state status;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	status = goose_prepare(tester.connection, "select cast(111 as short) * $1", &stmt);
	REQUIRE(status == GooseSuccess);
	REQUIRE(stmt != nullptr);

	status = goose_bind_int64(stmt, 1, 1665);
	REQUIRE(status == GooseSuccess);

	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 184815);
	goose_destroy_result(&res);
	goose_destroy_prepare(&stmt);
}

TEST_CASE("Prepared streaming result", "[capi]") {
	CAPITester tester;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	SECTION("non streaming result") {
		REQUIRE(tester.Query("CREATE TABLE t2 (i INTEGER, j INTEGER);"));

		goose_prepared_statement stmt;
		REQUIRE(goose_prepare(tester.connection,
		                       "INSERT INTO t2  SELECT 2 AS i, 3 AS j  RETURNING *, i * j AS i_times_j",
		                       &stmt) == GooseSuccess);
		goose_result res;
		REQUIRE(goose_execute_prepared_streaming(stmt, &res) == GooseSuccess);
		REQUIRE(!goose_result_is_streaming(res));
		goose_destroy_result(&res);
		goose_destroy_prepare(&stmt);
	}

	SECTION("streaming result") {
		goose_prepared_statement stmt;
		REQUIRE(goose_prepare(tester.connection, "FROM RANGE(0, 10)", &stmt) == GooseSuccess);

		goose_result res;
		REQUIRE(goose_execute_prepared_streaming(stmt, &res) == GooseSuccess);
		REQUIRE(goose_result_is_streaming(res));

		goose_data_chunk chunk;
		idx_t index = 0;
		while (true) {
			chunk = goose_stream_fetch_chunk(res);
			if (!chunk) {
				break;
			}
			auto chunk_size = goose_data_chunk_get_size(chunk);
			REQUIRE(chunk_size > 0);

			auto vec = goose_data_chunk_get_vector(chunk, 0);
			auto column_type = goose_vector_get_column_type(vec);
			REQUIRE(goose_get_type_id(column_type) == GOOSE_TYPE_BIGINT);
			goose_destroy_logical_type(&column_type);

			auto data = reinterpret_cast<int64_t *>(goose_vector_get_data(vec));
			for (idx_t i = 0; i < chunk_size; i++) {
				REQUIRE(data[i] == int64_t(index + i));
			}
			index += chunk_size;
			goose_destroy_data_chunk(&chunk);
		}

		REQUIRE(goose_stream_fetch_chunk(res) == nullptr);

		goose_destroy_result(&res);
		goose_destroy_prepare(&stmt);
	}

	SECTION("streaming extracted statements") {
		goose_extracted_statements stmts;
		auto n_statements = goose_extract_statements(tester.connection, "Select 1; Select 2;", &stmts);
		REQUIRE(n_statements == 2);

		for (idx_t i = 0; i < n_statements; i++) {
			goose_prepared_statement stmt;
			REQUIRE(goose_prepare_extracted_statement(tester.connection, stmts, i, &stmt) == GooseSuccess);

			goose_result res;
			REQUIRE(goose_execute_prepared_streaming(stmt, &res) == GooseSuccess);
			REQUIRE(goose_result_is_streaming(res));

			goose_data_chunk chunk;
			chunk = goose_stream_fetch_chunk(res);
			REQUIRE(chunk != nullptr);
			REQUIRE(goose_data_chunk_get_size(chunk) == 1);

			auto vec = goose_data_chunk_get_vector(chunk, 0);

			auto type = goose_vector_get_column_type(vec);
			REQUIRE(goose_get_type_id(type) == GOOSE_TYPE_INTEGER);
			goose_destroy_logical_type(&type);

			auto data = (int32_t *)goose_vector_get_data(vec);
			REQUIRE(data[0] == (int32_t)(i + 1));

			REQUIRE(goose_stream_fetch_chunk(res) == nullptr);

			goose_destroy_data_chunk(&chunk);
			goose_destroy_result(&res);
			goose_destroy_prepare(&stmt);
		}

		goose_destroy_extracted(&stmts);
	}
}

TEST_CASE("Test STRING LITERAL parameter type", "[capi]") {
	goose_database db;
	goose_connection conn;
	goose_prepared_statement stmt;

	REQUIRE(goose_open("", &db) == GooseSuccess);
	REQUIRE(goose_connect(db, &conn) == GooseSuccess);

	REQUIRE(goose_prepare(conn, "SELECT ?", &stmt) == GooseSuccess);
	REQUIRE(goose_bind_varchar(stmt, 1, "a") == GooseSuccess);
	REQUIRE(goose_param_type(stmt, 1) == GOOSE_TYPE_STRING_LITERAL);
	goose_destroy_prepare(&stmt);

	goose_disconnect(&conn);
	goose_close(&db);
}
