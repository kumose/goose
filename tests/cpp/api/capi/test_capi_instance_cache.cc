#include <goose/testing/capi_tester.h>
#include <goose/goose-c.h>
#include <goose/testing/test_helpers.h>

#include <chrono>
#include <thread>

using namespace goose;
using namespace std;

static void background_thread_connect(goose_instance_cache instance_cache, const char *path) {
	try {
		goose_database out_database;
		auto state = goose_get_or_create_from_cache(instance_cache, path, &out_database, nullptr, nullptr);
		REQUIRE(state == GooseSuccess);
		goose_close(&out_database);
	} catch (std::exception &ex) {
		FAIL(ex.what());
	}
}

TEST_CASE("Test the database instance cache in the C API", "[api][.]") {
	auto instance_cache = goose_create_instance_cache();

	for (idx_t i = 0; i < 30; i++) {
		auto path = TestCreatePath("shared_db.db");

		goose_database shared_out_database;
		auto state =
		    goose_get_or_create_from_cache(instance_cache, path.c_str(), &shared_out_database, nullptr, nullptr);
		REQUIRE(state == GooseSuccess);

		thread background_thread(background_thread_connect, instance_cache, path.c_str());
		goose_close(&shared_out_database);
		background_thread.join();
		TestDeleteFile(path);
		REQUIRE(1);
	}

	goose_destroy_instance_cache(&instance_cache);
}

TEST_CASE("Test the database instance cache in the C API with a null path", "[capi]") {
	auto instance_cache = goose_create_instance_cache();
	goose_database db;
	auto state = goose_get_or_create_from_cache(instance_cache, nullptr, &db, nullptr, nullptr);
	REQUIRE(state == GooseSuccess);
	goose_close(&db);
	goose_destroy_instance_cache(&instance_cache);
}

TEST_CASE("Test the database instance cache in the C API with an empty path", "[capi]") {
	auto instance_cache = goose_create_instance_cache();
	goose_database db;
	auto state = goose_get_or_create_from_cache(instance_cache, "", &db, nullptr, nullptr);
	REQUIRE(state == GooseSuccess);
	goose_close(&db);
	goose_destroy_instance_cache(&instance_cache);
}

TEST_CASE("Test the database instance cache in the C API with a memory path", "[capi]") {
	auto instance_cache = goose_create_instance_cache();
	goose_database db;
	auto state = goose_get_or_create_from_cache(instance_cache, ":memory:", &db, nullptr, nullptr);
	REQUIRE(state == GooseSuccess);
	goose_close(&db);
	goose_destroy_instance_cache(&instance_cache);
}

TEST_CASE("Test the database instance cache with case-insensitive FS", "[capi]") {
	// Check that file on disk is created in user-specified case
	// independently whether FS is case-sensitive or not.
	// FS access can be eventually changed to CAPI FS.
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	string work_dir = TestDirectoryPath();
	auto instance_cache = goose_create_instance_cache();

	std::vector<string> db_file_list = {"Test1.db", "TEST2.db", "TeSt3.db"};
	for (const string &db_file : db_file_list) {
		string db_path = TestJoinPath(work_dir, db_file);
		string attached_db_path = TestJoinPath(work_dir, "attached_" + db_file);
		fs->TryRemoveFile(db_path);
		fs->TryRemoveFile(attached_db_path);

		goose_database db1 = nullptr;
		REQUIRE(goose_get_or_create_from_cache(instance_cache, db_path.c_str(), &db1, nullptr, nullptr) ==
		        GooseSuccess);
		goose_connection conn1 = nullptr;
		REQUIRE(goose_connect(db1, &conn1) == GooseSuccess);
		REQUIRE(goose_query(conn1, (string("ATTACH '") + attached_db_path + "' AS attached").c_str(), nullptr) ==
		        GooseSuccess);
		REQUIRE(goose_query(conn1, "CREATE TABLE attached.tab1 (col1 INT)", nullptr) == GooseSuccess);

		// Check the file with originally specified case exists in FS
		bool found = false;
		fs->ListFiles(work_dir, [&db_file, &found](const string &name, bool) {
			if (name == db_file) {
				found = true;
			}
		});
		REQUIRE(found);

		// Get the DB from cache using different case and check that instance is the same
		// (when the FS and instance cache are case-insensitive) and different (when case-sensitive)
		string db_file_lower = db_file;
		std::transform(db_file_lower.begin(), db_file_lower.end(), db_file_lower.begin(),
		               [](unsigned char c) { return tolower(c); });
		string db_path_lower = TestJoinPath(work_dir, db_file_lower);
		fs->TryRemoveFile(db_path_lower);
		goose_database db2 = nullptr;
		REQUIRE(goose_get_or_create_from_cache(instance_cache, db_path_lower.c_str(), &db2, nullptr, nullptr) ==
		        GooseSuccess);
		goose_connection conn2 = nullptr;
		REQUIRE(goose_connect(db2, &conn2) == GooseSuccess);
		goose_result result;
		memset(&result, '\0', sizeof(result));
		REQUIRE(goose_query(conn2, "SELECT COUNT(*) FROM information_schema.tables WHERE table_name = 'tab1'",
		                     &result) == GooseSuccess);
		goose_data_chunk chunk = goose_fetch_chunk(result);
		REQUIRE(chunk != nullptr);
		goose_vector vec = goose_data_chunk_get_vector(chunk, 0);
		REQUIRE(vec != nullptr);
		int32_t *data = reinterpret_cast<int32_t *>(goose_vector_get_data(vec));
		int32_t count = data[0];
#if defined(_WIN32) || defined(__APPLE__) // case insensitive, attached
		REQUIRE(count == 1);
#else  // !(_WIN32 or __APPLE__): case sensitive, not attached
		REQUIRE(count == 0);
#endif // _WIN32 or __APPLE__
       // Cleanup
		goose_destroy_data_chunk(&chunk);
		goose_destroy_result(&result);
		goose_disconnect(&conn2);
		goose_close(&db2);
		goose_disconnect(&conn1);
		goose_close(&db1);
		if (count == 0) {
			fs->TryRemoveFile(db_path_lower);
		}
		fs->TryRemoveFile(db_path);
		fs->TryRemoveFile(attached_db_path);
	}
	goose_destroy_instance_cache(&instance_cache);
}
