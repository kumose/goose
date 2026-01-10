#include "capi_tester.h"
#include <goose/goose-c.h>

using namespace goose;
using namespace std;

TEST_CASE("Test pending statements in C API", "[capi]") {
	CAPITester tester;
	CAPIPrepared prepared;
	CAPIPending pending;
	goose::unique_ptr<CAPIResult> result;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));
	REQUIRE(prepared.Prepare(tester, "SELECT SUM(i) FROM range(1000000) tbl(i)"));
	REQUIRE(pending.Pending(prepared));

	while (true) {
		auto state = pending.ExecuteTask();
		REQUIRE(state != GOOSE_PENDING_ERROR);
		if (goose_pending_execution_is_finished(state)) {
			break;
		}
	}

	result = pending.Execute();
	REQUIRE(result);
	REQUIRE(!result->HasError());
	REQUIRE(result->Fetch<int64_t>(0, 0) == 499999500000LL);
}
