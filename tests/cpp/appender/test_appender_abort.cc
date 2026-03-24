#include <goose/testing/catch.h>
#include <goose/main/appender.h>
#include <goose/testing/test_helpers.h>

#include <vector>

using namespace goose;
using namespace std;

TEST_CASE("Abort appender due to primary key conflict", "[appender]") {
	goose::unique_ptr<QueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	REQUIRE_NO_FAIL(con.Query("CREATE TABLE integers(i INTEGER PRIMARY KEY)"));
	REQUIRE_NO_FAIL(con.Query("INSERT INTO integers VALUES (1)"));

	Appender appender(con, "integers");
	appender.BeginRow();
	appender.Append<int32_t>(1);
	appender.EndRow();
	// this should fail!
	REQUIRE_THROWS(appender.Flush());

	result = con.Query("SELECT * FROM integers");
	REQUIRE(CHECK_COLUMN(result, 0, {1}));
}
