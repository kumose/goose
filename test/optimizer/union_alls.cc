#include "catch.h"
#include "test_helpers.h"
#include <goose/goose.h>
#include <goose/main/database.h>
#include <goose/main/secret/secret_manager.h>
#include <goose/main/secret/secret_storage.h>
#include <goose/main/secret/secret.h>

using namespace goose;
using namespace std;

TEST_CASE("A lot of unions", "[optimizer][.]") {
	Goose db(nullptr);
	Connection con(db);

	goose::stringstream q;
	q << "select 1 as num";

	int max = 0;
	int curr = 1;
	while (max < 500) {
		max = (max * 1.1) + 1;
		while (curr < max) {
			q << " union all select 1 as num";
			curr++;
		}
		REQUIRE_NO_FAIL(con.Query(q.str()));
	}
}
