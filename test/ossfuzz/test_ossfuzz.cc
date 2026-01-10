#include "catch.h"
#include <goose/common/file_system.h>
#include "test_helpers.h"

#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>

using namespace goose;
using namespace std;

constexpr const char *QUERY_DIRECTORY = "test/ossfuzz/cases";

static void test_runner() {
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	auto file_name = Catch::getResultCapture().getCurrentTestName();

	goose::unique_ptr<QueryResult> result;
	Goose db(nullptr);
	Connection con(db);
	std::ifstream t(file_name);
	goose::stringstream buffer;
	buffer << t.rdbuf();
	auto query = buffer.str();
	result = con.Query(query.c_str());

	unordered_set<string> internal_error_messages = {"Unoptimized Result differs from original result!", "INTERNAL"};
	if (result->HasError()) {
		if (TestIsInternalError(internal_error_messages, result->GetError())) {
			result->Print();
			REQUIRE(!result->GetErrorObject().HasError());
		}
	}

	// we don't know whether the query fails or not and we don't know the
	// correct result we just don't want it to crash
	REQUIRE(1 == 1);
}

struct RegisterOssfuzzTests {
	RegisterOssfuzzTests() {
		// register a separate test for each file in the QUERY_DIRECTORY
		goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
		fs->ListFiles(QUERY_DIRECTORY, [&](string path, bool) {
			REGISTER_TEST_CASE(test_runner, string(QUERY_DIRECTORY) + "/" + path, "[ossfuzz][.]");
		});
	}
};
RegisterOssfuzzTests register_ossfuzz_test;
