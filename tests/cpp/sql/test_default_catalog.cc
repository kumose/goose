#include <goose/testing/catch.h>
#include <goose/common/value_operations/value_operations.h>
#include <goose/testing/test_helpers.h>

#include <atomic>
#include <random>
#include <thread>

using namespace goose;
using namespace std;

class ConcurrentDefaultCatalog {
public:
	static constexpr int CONCURRENT_DEFAULT_THREAD_COUNT = 10;
	static constexpr int CONCURRENT_DEFAULT_ITERATION_COUNT = 10;

	static void ScanDefaultCatalog(Goose *db, bool *read_correct) {
		Connection con(*db);
		*read_correct = true;
		for (idx_t i = 0; i < CONCURRENT_DEFAULT_ITERATION_COUNT; i++) {
			auto result = con.Query("SELECT * FROM pg_class");
			if (result->HasError()) {
				*read_correct = false;
			}
		}
	}

	static void QueryDefaultCatalog(Goose *db, bool *read_correct, int thread_id) {
		goose::vector<string> random_default_views {"pragma_database_list", "sqlite_master",      "sqlite_schema",
		                                             "sqlite_temp_master",   "sqlite_temp_schema", "goose_constraints",
		                                             "goose_columns",       "goose_indexes",     "goose_schemas",
		                                             "goose_tables",        "goose_types",       "goose_views"};

		Connection con(*db);
		*read_correct = true;
		for (idx_t i = 0; i < CONCURRENT_DEFAULT_ITERATION_COUNT; i++) {
			auto result = con.Query("SELECT * FROM " + random_default_views[rand() % random_default_views.size()]);
			if (result->HasError()) {
				*read_correct = false;
			}
		}
	}

	static void QueryDefaultCatalogFunctions(Goose *db, bool *read_correct, int thread_id) {
		goose::vector<string> random_queries {
		    "SELECT pg_collation_is_visible(0)",
		    "SELECT pg_conversion_is_visible(0)",
		    "SELECT pg_function_is_visible(0)",
		    "SELECT pg_opclass_is_visible(0)",
		    "SELECT pg_operator_is_visible(0)",
		    "SELECT pg_opfamily_is_visible(0)",
		    "SELECT pg_table_is_visible(0)",
		    "SELECT pg_ts_config_is_visible(0)",
		    "SELECT pg_ts_dict_is_visible(0)",
		    "SELECT pg_ts_parser_is_visible(0)",
		    "SELECT pg_ts_template_is_visible(0)",
		    "SELECT pg_type_is_visible(0)",
		    "SELECT current_user",
		    "SELECT current_catalog",
		    "SELECT current_database()",
		    "SELECT user",
		    "SELECT session_user",
		    "SELECT inet_client_addr()",
		    "SELECT inet_client_port()",
		    "SELECT inet_server_addr()",
		    "SELECT inet_server_port()",
		    "SELECT pg_my_temp_schema()",
		};

		Connection con(*db);
		*read_correct = true;
		for (idx_t i = 0; i < CONCURRENT_DEFAULT_ITERATION_COUNT; i++) {
			auto result = con.Query(random_queries[rand() % random_queries.size()]);
			if (result->HasError()) {
				*read_correct = false;
			}
		}
	}
};

TEST_CASE("Concurrent default catalog using Scan", "[interquery][.]") {
	goose::unique_ptr<MaterializedQueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	// enable detailed profiling
	con.Query("PRAGMA enable_profiling");
	auto detailed_profiling_output = TestCreatePath("detailed_profiling_output.txt");
	con.Query("PRAGMA profiling_output='" + detailed_profiling_output + "'");
	con.Query("PRAGMA profiling_mode = detailed");

	bool correct[ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT];
	thread threads[ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT];
	for (size_t i = 0; i < ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT; i++) {
		threads[i] = thread(ConcurrentDefaultCatalog::ScanDefaultCatalog, &db, correct + i);
	}

	for (size_t i = 0; i < ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT; i++) {
		threads[i].join();
		REQUIRE(correct[i]);
	}
}

TEST_CASE("Concurrent default catalog using Queries", "[interquery][.]") {
	goose::unique_ptr<MaterializedQueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	// enable detailed profiling
	con.Query("PRAGMA enable_profiling");
	auto detailed_profiling_output = TestCreatePath("detailed_profiling_output.txt");
	con.Query("PRAGMA profiling_output='" + detailed_profiling_output + "'");
	con.Query("PRAGMA profiling_mode = detailed");

	bool correct[ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT];
	thread threads[ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT];
	for (size_t i = 0; i < ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT; i++) {
		threads[i] = thread(ConcurrentDefaultCatalog::QueryDefaultCatalog, &db, correct + i, i);
	}

	for (size_t i = 0; i < ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT; i++) {
		threads[i].join();
		REQUIRE(correct[i]);
	}
}

TEST_CASE("Concurrent default function creation", "[interquery][.]") {
	goose::unique_ptr<MaterializedQueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	// enable detailed profiling
	con.Query("PRAGMA enable_profiling");
	auto detailed_profiling_output = TestCreatePath("detailed_profiling_output.txt");
	con.Query("PRAGMA profiling_output='" + detailed_profiling_output + "'");
	con.Query("PRAGMA profiling_mode = detailed");

	bool correct[ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT];
	thread threads[ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT];
	for (size_t i = 0; i < ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT; i++) {
		threads[i] = thread(ConcurrentDefaultCatalog::QueryDefaultCatalogFunctions, &db, correct + i, i);
	}

	for (size_t i = 0; i < ConcurrentDefaultCatalog::CONCURRENT_DEFAULT_THREAD_COUNT; i++) {
		threads[i].join();
		REQUIRE(correct[i]);
	}
}
