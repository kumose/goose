#include <goose/testing/catch.h>
#include <goose/main/appender.h>
#include <goose/testing/test_helpers.h>

#include <vector>

using namespace goose;
using namespace std;

TEST_CASE("Test the way appenders interact with transactions", "[appender]") {
	goose::unique_ptr<QueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	REQUIRE_NO_FAIL(con.Query("CREATE TABLE integers(i INTEGER PRIMARY KEY)"));

	// begin a transaction manually
	REQUIRE_NO_FAIL(con.Query("BEGIN TRANSACTION"));

	// append a value to the table
	Appender appender(con, "integers");

	appender.BeginRow();
	appender.Append<int32_t>(1);
	appender.EndRow();

	appender.Close();

	// we can select the value now
	result = con.Query("SELECT * FROM integers");
	REQUIRE(CHECK_COLUMN(result, 0, {1}));

	// rolling back cancels the transaction
	REQUIRE_NO_FAIL(con.Query("ROLLBACK"));

	// now the values are gone
	result = con.Query("SELECT * FROM integers");
	REQUIRE(CHECK_COLUMN(result, 0, {}));
}
