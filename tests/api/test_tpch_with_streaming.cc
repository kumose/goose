#include <goose/testing/catch.h>
#include <goose/testing/test_helpers.h>
#include "tpch_extension.hpp"

#include <chrono>
#include <iostream>
#include <goose/common/string_util.h>

using namespace goose;
using namespace std;

TEST_CASE("Test TPC-H SF0.01 using streaming api", "[tpch][.]") {
	goose::unique_ptr<QueryResult> result;
	Goose db(nullptr);
	Connection con(db);
	double sf = 0.01;
	if (!db.ExtensionIsLoaded("tpch")) {
		return;
	}

	REQUIRE_NO_FAIL(con.Query("CALL dbgen(sf=" + to_string(sf) + ")"));

	for (idx_t tpch_num = 1; tpch_num <= 22; tpch_num++) {
		result = con.SendQuery("pragma tpch(" + to_string(tpch_num) + ");");

		goose::ColumnDataCollection collection(goose::Allocator::DefaultAllocator(), result->types);

		while (true) {
			auto chunk = result->Fetch();
			if (chunk) {
				collection.Append(*chunk);
			} else {
				break;
			}
		}

		COMPARE_CSV_COLLECTION(collection, TpchExtension::GetAnswer(sf, tpch_num), true);
	}
}
