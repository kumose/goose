#include "capi_tester.h"

using namespace goose;
using namespace std;

TEST_CASE("Test C API examples from the website", "[capi]") {
	// NOTE: if any of these break and need to be changed, the website also needs to be updated!
	SECTION("connect") {
		goose_database db;
		goose_connection con;

		if (goose_open(NULL, &db) == GooseError) {
			// handle error
		}
		if (goose_connect(db, &con) == GooseError) {
			// handle error
		}

		// run queries...

		// cleanup
		goose_disconnect(&con);
		goose_close(&db);
	}
	SECTION("config") {
		goose_database db;
		goose_config config;

		// create the configuration object
		if (goose_create_config(&config) == GooseError) {
			REQUIRE(1 == 0);
		}
		// set some configuration options
		goose_set_config(config, "access_mode", "READ_WRITE");
		goose_set_config(config, "threads", "8");
		goose_set_config(config, "max_memory", "8GB");
		goose_set_config(config, "default_order", "DESC");

		// open the database using the configuration
		if (goose_open_ext(NULL, &db, config, NULL) == GooseError) {
			REQUIRE(1 == 0);
		}
		// cleanup the configuration object
		goose_destroy_config(&config);

		// run queries...

		// cleanup
		goose_close(&db);
	}
	SECTION("query") {
		goose_database db;
		goose_connection con;
		goose_state state;
		goose_result result;

		goose_open(NULL, &db);
		goose_connect(db, &con);

		// create a table
		state = goose_query(con, "CREATE TABLE integers(i INTEGER, j INTEGER);", NULL);
		if (state == GooseError) {
			REQUIRE(1 == 0);
		}
		// insert three rows into the table
		state = goose_query(con, "INSERT INTO integers VALUES (3, 4), (5, 6), (7, NULL);", NULL);
		if (state == GooseError) {
			REQUIRE(1 == 0);
		}
		// query rows again
		state = goose_query(con, "SELECT * FROM integers", &result);
		if (state == GooseError) {
			REQUIRE(1 == 0);
		}
		// handle the result
		idx_t row_count = goose_row_count(&result);
		idx_t column_count = goose_column_count(&result);
		for (idx_t row = 0; row < row_count; row++) {
			for (idx_t col = 0; col < column_count; col++) {
				// if (col > 0) printf(",");
				auto str_val = goose_value_varchar(&result, col, row);
				// printf("%s", str_val);
				REQUIRE(1 == 1);
				goose_free(str_val);
			}
			//	printf("\n");
		}

		int32_t *i_data = (int32_t *)goose_column_data(&result, 0);
		int32_t *j_data = (int32_t *)goose_column_data(&result, 1);
		bool *i_mask = goose_nullmask_data(&result, 0);
		bool *j_mask = goose_nullmask_data(&result, 1);
		for (idx_t row = 0; row < row_count; row++) {
			if (i_mask[row]) {
				// printf("NULL");
			} else {
				REQUIRE(i_data[row] > 0);
				// printf("%d", i_data[row]);
			}
			// printf(",");
			if (j_mask[row]) {
				// printf("NULL");
			} else {
				REQUIRE(j_data[row] > 0);
				// printf("%d", j_data[row]);
			}
			// printf("\n");
		}

		// destroy the result after we are done with it
		goose_destroy_result(&result);
		goose_disconnect(&con);
		goose_close(&db);
	}
	SECTION("prepared") {
		goose_database db;
		goose_connection con;
		goose_open(NULL, &db);
		goose_connect(db, &con);
		goose_query(con, "CREATE TABLE integers(i INTEGER, j INTEGER)", NULL);

		goose_prepared_statement stmt;
		goose_result result;
		if (goose_prepare(con, "INSERT INTO integers VALUES ($1, $2)", &stmt) == GooseError) {
			REQUIRE(1 == 0);
		}

		goose_bind_int32(stmt, 1, 42); // the parameter index starts counting at 1!
		goose_bind_int32(stmt, 2, 43);
		// NULL as second parameter means no result set is requested
		goose_execute_prepared(stmt, NULL);
		goose_destroy_prepare(&stmt);

		// we can also query result sets using prepared statements
		if (goose_prepare(con, "SELECT * FROM integers WHERE i = ?", &stmt) == GooseError) {
			REQUIRE(1 == 0);
		}
		goose_bind_int32(stmt, 1, 42);
		goose_execute_prepared(stmt, &result);

		// do something with result

		// clean up
		goose_destroy_result(&result);
		goose_destroy_prepare(&stmt);

		goose_disconnect(&con);
		goose_close(&db);
	}
	SECTION("appender") {
		goose_database db;
		goose_connection con;
		goose_open(NULL, &db);
		goose_connect(db, &con);
		goose_query(con, "CREATE TABLE people(id INTEGER, name VARCHAR)", NULL);

		goose_appender appender;
		if (goose_appender_create(con, NULL, "people", &appender) == GooseError) {
			REQUIRE(1 == 0);
		}
		goose_append_int32(appender, 1);
		goose_append_varchar(appender, "Mark");
		goose_appender_end_row(appender);

		goose_append_int32(appender, 2);
		goose_append_varchar(appender, "Hannes");
		goose_appender_end_row(appender);

		goose_appender_destroy(&appender);

		goose_result result;
		goose_query(con, "SELECT * FROM people", &result);
		REQUIRE(goose_value_int32(&result, 0, 0) == 1);
		REQUIRE(goose_value_int32(&result, 0, 1) == 2);
		REQUIRE(string(goose_value_varchar_internal(&result, 1, 0)) == "Mark");
		REQUIRE(string(goose_value_varchar_internal(&result, 1, 1)) == "Hannes");

		// error conditions: we cannot
		REQUIRE(goose_value_varchar_internal(&result, 0, 0) == nullptr);
		REQUIRE(goose_value_varchar_internal(nullptr, 0, 0) == nullptr);

		goose_destroy_result(&result);

		goose_disconnect(&con);
		goose_close(&db);
	}
}
