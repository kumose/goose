#ifdef _WIN32
#include <windows.h>
#endif
#include "catch.h"
#include "test_helpers.h"
#include <goose/goose.h>

using namespace goose;
using namespace std;

TEST_CASE("Test compatibility with windows.h", "[windows]") {
	Goose db(nullptr);
	Connection con(db);

	// This test solely exists to check if compilation is hindered by including windows.h
	// before including goose.h
	con.BeginTransaction();
	con.Query("select 42;");
	con.Commit();
}
