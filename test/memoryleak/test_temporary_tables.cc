#include "catch.h"
#include <goose/common/file_system.h>
#include <goose/storage/buffer_manager.h>
#include <goose/storage/storage_info.h>
#include "test_helpers.h"
#include <goose/main/client_context.h>
#include <goose/main/config.h>
#include "test_config.h"

using namespace goose;
using namespace std;

TEST_CASE("Test in-memory database scanning from tables", "[memoryleak]") {
	if (!TestConfiguration::TestMemoryLeaks()) {
		return;
	}
	Goose db;
	Connection con(db);
	REQUIRE_NO_FAIL(
	    con.Query("create table t1 as select i, concat('thisisalongstring', i) s from range(1000000) t(i);"));
	while (true) {
		REQUIRE_NO_FAIL(con.Query("SELECT * FROM t1"));
	}
}

TEST_CASE("Rollback create table", "[memoryleak]") {
	if (!TestConfiguration::TestMemoryLeaks()) {
		return;
	}
	DBConfig config;
	config.options.load_extensions = false;
	Goose db(":memory:", &config);
	Connection con(db);
	while (true) {
		REQUIRE_NO_FAIL(con.Query("BEGIN"));
		REQUIRE_NO_FAIL(con.Query("CREATE TABLE t2(i INT);"));
		REQUIRE_NO_FAIL(con.Query("ROLLBACK"));
	}
}

TEST_CASE("DB temporary table insertion", "[memoryleak]") {
	if (!TestConfiguration::TestMemoryLeaks()) {
		return;
	}
	auto db_path = TestCreatePath("memory_leak_temp_table.db");
	DeleteDatabase(db_path);

	Goose db(db_path);
	{
		Connection con(db);
		REQUIRE_NO_FAIL(con.Query("SET threads=8;"));
		REQUIRE_NO_FAIL(con.Query("SET preserve_insertion_order=false;"));
		REQUIRE_NO_FAIL(con.Query("SET force_compression='uncompressed';"));
		REQUIRE_NO_FAIL(con.Query("create table t1 as select i from range(1000000) t(i);"));
	}
	Connection con(db);
	while (true) {
		REQUIRE_NO_FAIL(con.Query("BEGIN"));
		REQUIRE_NO_FAIL(con.Query("CREATE OR REPLACE TEMPORARY TABLE t2(i int)"));
		REQUIRE_NO_FAIL(con.Query("insert into t2 SELECT * FROM t1;"));
		REQUIRE_NO_FAIL(con.Query("ROLLBACk"));
	}
}

// FIXME: broken right now - we need to flush/merge deletes to fix this
// TEST_CASE("Insert and delete data repeatedly", "[memoryleak]") {
//	if (!TestMemoryLeaks()) {
//		return;
//	}
//	DBConfig config;
//	config.options.load_extensions = false;
//	Goose db(":memory:", &config);
//	Connection con(db);
//	REQUIRE_NO_FAIL(con.Query("CREATE TABLE t1(i INT);"));
//	while (true) {
//		REQUIRE_NO_FAIL(con.Query("INSERT INTO t1 SELECT * FROM range(100000)"));
//		REQUIRE_NO_FAIL(con.Query("DELETE FROM t1"));
//	}
//}
