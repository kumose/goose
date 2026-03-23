#include <goose/testing/catch.h>
#include <goose/testing/test_helpers.h>
#include <goose/main/config.h>
#include <goose/testing/test_config.h>

using namespace goose;
using namespace std;

void rand_str(char *dest, idx_t length) {
	char charset[] = "0123456789"
	                 "abcdefghijklmnopqrstuvwxyz"
	                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	while (length-- > 0) {
		idx_t index = (double)rand() / RAND_MAX * (sizeof charset - 1);
		*dest++ = charset[index];
	}
	*dest = '\0';
}

TEST_CASE("Test repeated appending small chunks to a table", "[memoryleak]") {
	if (!TestConfiguration::TestMemoryLeaks()) {
		return;
	}
	goose_database db;
	goose_connection con;
	goose_state state;
	auto db_path = TestCreatePath("appender_leak_test.db");
	TestDeleteFile(db_path);

	if (goose_open(db_path.c_str(), &db) == GooseError) {
		// handle error
		FAIL("Failed to open");
	}
	if (goose_connect(db, &con) == GooseError) {
		// handle error
		FAIL("Failed to connect");
	}

	state =
	    goose_query(con, "create table test(col1 varchar, col2 varchar, col3 bigint, col4 bigint, col5 double)", NULL);
	if (state == GooseError) {
		FAIL("Failed to create table");
	}
	state = goose_query(con, "set memory_limit='100mb'", NULL);
	if (state == GooseError) {
		FAIL("Failed to set memory limit");
	}

	long n1 = 0;
	double d1 = 0.5;
	for (int i = 0; i < 100000; i++) {
		goose_appender appender;
		if (goose_appender_create(con, NULL, "test", &appender) == GooseError) {
			FAIL("Failed to create appender");
		}
		for (int j = 0; j < 1000; j++) {
			char str[41];
			rand_str(str, sizeof(str) - 1);
			goose_append_varchar(appender, str);
			goose_append_varchar(appender, "hello");
			goose_append_int64(appender, n1++);
			goose_append_int64(appender, n1++);
			goose_append_double(appender, d1);
			d1 += 1.25;
			goose_appender_end_row(appender);
		}
		state = goose_appender_close(appender);
		if (state == GooseError) {
			FAIL("Failed to close appender");
		}
		state = goose_appender_destroy(&appender);
		if (state == GooseError) {
			fprintf(stderr, "err: %d", state);
			FAIL("Failed to destroy appender");
		}
		if (i % 500 == 0) {
			printf("completed %d\n", i);
			goose_query(con, "checkpoint", NULL);
		}
	}

	// cleanup
	goose_disconnect(&con);
	goose_close(&db);
	REQUIRE(1 == 1);
}
