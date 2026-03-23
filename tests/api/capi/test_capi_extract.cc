#include "capi_tester.h"

using namespace goose;
using namespace std;

TEST_CASE("Test extract statements in C API", "[capi]") {
	CAPITester tester;
	goose_result res;
	goose_extracted_statements stmts = nullptr;
	goose_state status;
	const char *error;
	goose_prepared_statement prepared = nullptr;

	REQUIRE(tester.OpenDatabase(nullptr));

	idx_t size = goose_extract_statements(tester.connection,
	                                       "CREATE TABLE tbl (col INT); INSERT INTO tbl VALUES (1), (2), (3), (4); "
	                                       "SELECT COUNT(col) FROM tbl WHERE col > $1",
	                                       &stmts);

	REQUIRE(size == 3);
	REQUIRE(stmts != nullptr);

	for (idx_t i = 0; i + 1 < size; i++) {
		status = goose_prepare_extracted_statement(tester.connection, stmts, i, &prepared);
		REQUIRE(status == GooseSuccess);
		status = goose_execute_prepared(prepared, &res);
		REQUIRE(status == GooseSuccess);
		goose_destroy_prepare(&prepared);
		goose_destroy_result(&res);
	}

	goose_prepared_statement stmt = nullptr;
	status = goose_prepare_extracted_statement(tester.connection, stmts, size - 1, &stmt);
	REQUIRE(status == GooseSuccess);
	goose_bind_int32(stmt, 1, 1);
	status = goose_execute_prepared(stmt, &res);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_value_int64(&res, 0, 0) == 3);
	goose_destroy_prepare(&stmt);
	goose_destroy_result(&res);
	goose_destroy_extracted(&stmts);

	//	 test empty statement is not an error
	size = goose_extract_statements(tester.connection, "", &stmts);
	REQUIRE(size == 0);
	error = goose_extract_statements_error(stmts);
	REQUIRE(error == nullptr);
	goose_destroy_extracted(&stmts);

	//	 test incorrect statement cannot be extracted
	size = goose_extract_statements(tester.connection, "This is not valid SQL", &stmts);
	REQUIRE(size == 0);
	error = goose_extract_statements_error(stmts);
	REQUIRE(error != nullptr);
	goose_destroy_extracted(&stmts);

	// test out of bounds
	size = goose_extract_statements(tester.connection, "SELECT CAST($1 AS BIGINT)", &stmts);
	REQUIRE(size == 1);
	status = goose_prepare_extracted_statement(tester.connection, stmts, 2, &prepared);
	REQUIRE(status == GooseError);
	goose_destroy_extracted(&stmts);
}

TEST_CASE("Test invalid PRAGMA in C API", "[capi]") {
	goose_database db;
	goose_connection con;
	const char *err_msg;

	REQUIRE(goose_open(nullptr, &db) == GooseSuccess);
	REQUIRE(goose_connect(db, &con) == GooseSuccess);

	goose_extracted_statements stmts;
	auto size = goose_extract_statements(con, "PRAGMA something;", &stmts);

	REQUIRE(size == 0);
	err_msg = goose_extract_statements_error(stmts);
	REQUIRE(err_msg != nullptr);
	REQUIRE(string(err_msg).find("Catalog Error") != std::string::npos);

	goose_destroy_extracted(&stmts);
	goose_disconnect(&con);
	goose_close(&db);
}
