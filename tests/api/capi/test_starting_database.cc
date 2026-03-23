#include "capi_tester.h"
#include <goose/goose-c.h>
#include <goose/testing/test_helpers.h>

using namespace goose;
using namespace std;

TEST_CASE("Simple In-Memory DB Start Up and Shutdown", "[simplestartup]") {
	goose_database database;
	goose_connection connection;

	// open and close a database in in-memory mode
	REQUIRE(goose_open(NULL, &database) == GooseSuccess);
	REQUIRE(goose_connect(database, &connection) == GooseSuccess);
	goose_disconnect(&connection);
	goose_close(&database);
}

TEST_CASE("Multiple In-Memory DB Start Up and Shutdown", "[multiplestartup]") {
	goose_database database[10];
	goose_connection connection[100];

	// open and close 10 databases
	// and open 10 connections per database
	for (size_t i = 0; i < 10; i++) {
		REQUIRE(goose_open(NULL, &database[i]) == GooseSuccess);
		for (size_t j = 0; j < 10; j++) {
			REQUIRE(goose_connect(database[i], &connection[i * 10 + j]) == GooseSuccess);
		}
	}
	for (size_t i = 0; i < 10; i++) {
		for (size_t j = 0; j < 10; j++) {
			goose_disconnect(&connection[i * 10 + j]);
		}
		goose_close(&database[i]);
	}
}

TEST_CASE("On Disk DB File Name Case Preserved", "[simplestartup]") {
	// Check that file on disk is created in user-specified case
	// independently whether FS is case-sensitive or not.
	// FS access can be eventually changed to CAPI FS.
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	string work_dir = TestDirectoryPath();
	std::vector<string> db_file_list = {"Test1.db", "TEST2.db", "TeSt3.db"};
	for (const string &db_file : db_file_list) {
		string db_path = TestJoinPath(work_dir, db_file);
		fs->TryRemoveFile(db_path);
		{
			goose_database database = nullptr;
			REQUIRE(goose_open(db_path.c_str(), &database) == GooseSuccess);
			goose_connection conn = nullptr;
			REQUIRE(goose_connect(database, &conn) == GooseSuccess);
			REQUIRE(goose_query(conn, "CREATE TABLE tab1 (col1 INT)", nullptr) == GooseSuccess);
			goose_disconnect(&conn);
			goose_close(&database);
		}
		bool found = false;
		fs->ListFiles(work_dir, [&db_file, &found](const string &name, bool) {
			if (name == db_file) {
				found = true;
			}
		});
		REQUIRE(found);
		{
			// Open a connection to a lower-cased version of the path
			string db_file_lower = db_file;
			std::transform(db_file_lower.begin(), db_file_lower.end(), db_file_lower.begin(),
			               [](unsigned char c) { return tolower(c); });
			string db_path_lower = TestJoinPath(work_dir, db_file_lower);
			goose_database database;
			REQUIRE(goose_open(db_path_lower.c_str(), &database) == GooseSuccess);
			// Run a query to check that we've opened the existing DB file or created a new one
			// depending whether the FS is case-sensitive or not
			goose_connection conn = nullptr;
			REQUIRE(goose_connect(database, &conn) == GooseSuccess);
			goose_result result;
			memset(&result, '\0', sizeof(result));
			REQUIRE(goose_query(conn, "SELECT COUNT(*) FROM information_schema.tables WHERE table_name = 'tab1'",
			                     &result) == GooseSuccess);
			goose_data_chunk chunk = goose_fetch_chunk(result);
			REQUIRE(chunk != nullptr);
			goose_vector vec = goose_data_chunk_get_vector(chunk, 0);
			REQUIRE(vec != nullptr);
			int32_t *data = reinterpret_cast<int32_t *>(goose_vector_get_data(vec));
			int32_t count = data[0];
#if defined(_WIN32) || defined(__APPLE__) // case insensitive, same file
			REQUIRE(count == 1);
#else  // !(_WIN32 or __APPLE__): case sensitive, different files
			REQUIRE(count == 0);
#endif // _WIN32 or __APPLE__
			goose_destroy_data_chunk(&chunk);
			goose_destroy_result(&result);
			goose_disconnect(&conn);
			goose_close(&database);
			if (count == 0) {
				fs->TryRemoveFile(db_path_lower);
			}
		}
		fs->TryRemoveFile(db_path);
	}
}
