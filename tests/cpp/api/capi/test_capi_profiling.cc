#include <goose/testing/capi_tester.h>

using namespace goose;
using namespace std;

string BuildSettingsString(const goose::vector<string> &settings) {
	string result = "'{";
	for (idx_t i = 0; i < settings.size(); i++) {
		result += "\"" + settings[i] + "\": \"true\"";
		if (i < settings.size() - 1) {
			result += ", ";
		}
	}
	result += "}'";
	return result;
}

void RetrieveMetrics(goose_profiling_info info, goose::map<string, double> &cumulative_counter,
                     goose::map<string, double> &cumulative_result, const idx_t depth) {
	auto map = goose_profiling_info_get_metrics(info);
	REQUIRE(map);
	auto count = goose_get_map_size(map);
	REQUIRE(count != 0);

	// Test index out of bounds for MAP value.
	if (depth == 0) {
		auto invalid_key = goose_get_map_key(map, 10000000);
		REQUIRE(!invalid_key);
		auto invalid_value = goose_get_map_value(map, 10000000);
		REQUIRE(!invalid_value);
	}

	for (idx_t i = 0; i < count; i++) {
		auto key = goose_get_map_key(map, i);
		REQUIRE(key);
		auto value = goose_get_map_value(map, i);
		REQUIRE(value);

		auto key_c_str = goose_get_varchar(key);
		auto value_c_str = goose_get_varchar(value);
		auto key_str = goose::string(key_c_str);
		auto value_str = goose::string(value_c_str);

		if (depth == 0) {
			REQUIRE(key_str != EnumUtil::ToString(MetricType::OPERATOR_CARDINALITY));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::OPERATOR_ROWS_SCANNED));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::OPERATOR_TIMING));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::OPERATOR_NAME));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::OPERATOR_TYPE));
		} else {
			REQUIRE(key_str != EnumUtil::ToString(MetricType::QUERY_NAME));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::BLOCKED_THREAD_TIME));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::LATENCY));
			REQUIRE(key_str != EnumUtil::ToString(MetricType::ROWS_RETURNED));
		}

		if (key_str == EnumUtil::ToString(MetricType::QUERY_NAME) ||
		    key_str == EnumUtil::ToString(MetricType::OPERATOR_NAME) ||
		    key_str == EnumUtil::ToString(MetricType::OPERATOR_TYPE) ||
		    key_str == EnumUtil::ToString(MetricType::EXTRA_INFO)) {
			REQUIRE(!value_str.empty());
		} else {
			double result = 0;
			try {
				result = std::stod(value_str);
			} catch (std::invalid_argument &e) {
				REQUIRE(false);
			}

			if (cumulative_counter.find(key_str) != cumulative_counter.end()) {
				cumulative_counter[key_str] += result;
			}
			if (cumulative_result.find(key_str) != cumulative_result.end() && cumulative_result[key_str] == 0) {
				cumulative_result[key_str] = result;
			}
		}

		goose_destroy_value(&key);
		goose_destroy_value(&value);
		goose_free(key_c_str);
		goose_free(value_c_str);
	}

	goose_destroy_value(&map);
}

void TraverseTree(goose_profiling_info profiling_info, goose::map<string, double> &cumulative_counter,
                  goose::map<string, double> &cumulative_result, const idx_t depth) {
	RetrieveMetrics(profiling_info, cumulative_counter, cumulative_result, depth);

	// Recurse into the child node.
	auto child_count = goose_profiling_info_get_child_count(profiling_info);
	if (depth == 0) {
		REQUIRE(child_count != 0);
	}

	for (idx_t i = 0; i < child_count; i++) {
		auto child = goose_profiling_info_get_child(profiling_info, i);
		TraverseTree(child, cumulative_counter, cumulative_result, depth + 1);
	}
}

idx_t ConvertToInt(double value) {
	return idx_t(value * 1000);
}

TEST_CASE("Test profiling with a single metric and get_value", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));

	// test only CPU_TIME profiling
	goose::vector<string> settings = {"CPU_TIME"};
	REQUIRE_NO_FAIL(tester.Query("PRAGMA custom_profiling_settings=" + BuildSettingsString(settings)));
	REQUIRE_NO_FAIL(tester.Query("SELECT 42"));

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info != nullptr);
	// Retrieve a metric that is not enabled.
	REQUIRE(goose_profiling_info_get_value(info, "EXTRA_INFO") == nullptr);

	goose::map<string, double> cumulative_counter;
	goose::map<string, double> cumulative_result;

	TraverseTree(info, cumulative_counter, cumulative_result, 0);
	tester.Cleanup();
}

TEST_CASE("Test profiling with cumulative metrics", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));

	// test all profiling metrics
	goose::vector<string> settings = {"BLOCKED_THREAD_TIME",  "CPU_TIME",       "CUMULATIVE_CARDINALITY", "EXTRA_INFO",
	                                   "OPERATOR_CARDINALITY", "OPERATOR_TIMING"};
	REQUIRE_NO_FAIL(tester.Query("PRAGMA custom_profiling_settings=" + BuildSettingsString(settings)));
	REQUIRE_NO_FAIL(tester.Query("SELECT 42"));

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info != nullptr);

	goose::map<string, double> cumulative_counter = {{"OPERATOR_TIMING", 0}, {"OPERATOR_CARDINALITY", 0}};
	goose::map<string, double> cumulative_result {
	    {"CPU_TIME", 0},
	    {"CUMULATIVE_CARDINALITY", 0},
	};

	TraverseTree(info, cumulative_counter, cumulative_result, 0);

	REQUIRE(ConvertToInt(cumulative_result["CPU_TIME"]) == ConvertToInt(cumulative_counter["OPERATOR_TIMING"]));
	REQUIRE(ConvertToInt(cumulative_result["CUMULATIVE_CARDINALITY"]) ==
	        ConvertToInt(cumulative_counter["OPERATOR_CARDINALITY"]));
	tester.Cleanup();
}

TEST_CASE("Test profiling without profiling enabled", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	// Retrieve info without profiling enabled.
	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info == nullptr);
	tester.Cleanup();
}

TEST_CASE("Test profiling with detailed profiling mode enabled", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));
	REQUIRE_NO_FAIL(tester.Query("PRAGMA profiling_mode = 'detailed'"));
	REQUIRE_NO_FAIL(tester.Query("SELECT 42"));

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info != nullptr);

	goose::map<string, double> cumulative_counter;
	goose::map<string, double> cumulative_result;
	TraverseTree(info, cumulative_counter, cumulative_result, 0);
	tester.Cleanup();
}

TEST_CASE("Test invalid use of profiling API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));
	REQUIRE_NO_FAIL(tester.Query("PRAGMA profiling_mode = 'detailed'"));
	REQUIRE_NO_FAIL(tester.Query("SELECT 42"));

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info != nullptr);

	// Incorrect usage tests.

	auto map = goose_profiling_info_get_metrics(nullptr);
	REQUIRE(map == nullptr);
	map = goose_profiling_info_get_metrics(info);

	auto dummy_value = goose_create_bool(true);
	auto count = goose_get_map_size(nullptr);
	REQUIRE(count == 0);
	count = goose_get_map_size(dummy_value);
	REQUIRE(count == 0);
	count = goose_get_map_size(map);

	for (idx_t i = 0; i < count; i++) {
		auto key = goose_get_map_key(nullptr, i);
		REQUIRE(key == nullptr);
		key = goose_get_map_key(map, DConstants::INVALID_INDEX);
		REQUIRE(key == nullptr);
		key = goose_get_map_key(dummy_value, i);
		REQUIRE(key == nullptr);

		auto value = goose_get_map_value(nullptr, i);
		REQUIRE(value == nullptr);
		value = goose_get_map_value(map, DConstants::INVALID_INDEX);
		REQUIRE(value == nullptr);
		value = goose_get_map_value(dummy_value, i);
		REQUIRE(value == nullptr);
		break;
	}

	goose_destroy_value(&dummy_value);
	goose_destroy_value(&map);
	tester.Cleanup();
}

TEST_CASE("Test profiling after throwing an error", "[capi]") {
	CAPITester tester;
	auto main_db = TestCreatePath("profiling_error.db");
	REQUIRE(tester.OpenDatabase(main_db.c_str()));

	auto path = TestCreatePath("profiling_error.db");
	REQUIRE_NO_FAIL(tester.Query("ATTACH IF NOT EXISTS '" + path + "' (TYPE GOOSE)"));
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE profiling_error.tbl AS SELECT range AS id FROM range(10)"));

	REQUIRE_NO_FAIL(tester.Query("SET enable_profiling = 'no_output'"));
	REQUIRE_NO_FAIL(tester.Query("SET profiling_mode = 'standard'"));

	CAPIPrepared prepared_q1;
	CAPIPending pending_q1;
	REQUIRE(prepared_q1.Prepare(tester, "SELECT * FROM profiling_error.tbl"));
	REQUIRE(pending_q1.Pending(prepared_q1));
	auto result = pending_q1.Execute();
	REQUIRE(result);
	REQUIRE(!result->HasError());

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info != nullptr);

	CAPIPrepared prepared_q2;
	REQUIRE(!prepared_q2.Prepare(tester, "SELECT * FROM profiling_error.does_not_exist"));
	info = goose_get_profiling_info(tester.connection);
	REQUIRE(info == nullptr);

	tester.Cleanup();
}

TEST_CASE("Test profiling with Extra Info enabled", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));
	goose::vector<string> settings = {"EXTRA_INFO"};
	REQUIRE_NO_FAIL(tester.Query("PRAGMA custom_profiling_settings=" + BuildSettingsString(settings)));
	REQUIRE_NO_FAIL(tester.Query("SELECT 1"));

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info);

	// Retrieve the child node.
	auto child_info = goose_profiling_info_get_child(info, 0);
	REQUIRE(goose_profiling_info_get_child_count(child_info) != 0);

	auto map = goose_profiling_info_get_metrics(child_info);
	REQUIRE(map);
	auto count = goose_get_map_size(map);
	REQUIRE(count != 0);

	bool found_extra_info = false;
	for (idx_t i = 0; i < count; i++) {
		auto key = goose_get_map_key(map, i);
		REQUIRE(key);
		auto key_c_str = goose_get_varchar(key);
		auto key_str = goose::string(key_c_str);

		auto value = goose_get_map_value(map, i);
		REQUIRE(value);
		auto value_c_str = goose_get_varchar(value);
		auto value_str = goose::string(value_c_str);

		if (key_str == EnumUtil::ToString(MetricType::EXTRA_INFO)) {
			REQUIRE(value_str.find("__order_by__"));
			REQUIRE(value_str.find("ASC"));
			found_extra_info = true;
		}

		if (key) {
			goose_destroy_value(&key);
			goose_free(key_c_str);
		}
		if (value) {
			goose_destroy_value(&value);
			goose_free(value_c_str);
		}
	}

	REQUIRE(found_extra_info);

	goose_destroy_value(&map);
	tester.Cleanup();
}

TEST_CASE("Test profiling with the appender", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	tester.Query("CREATE TABLE tbl (i INT PRIMARY KEY, value VARCHAR)");
	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));
	REQUIRE_NO_FAIL(tester.Query("SET profiling_coverage='ALL'"));
	goose_appender appender;

	string query = "INSERT INTO tbl FROM my_appended_data";
	goose_logical_type types[2];
	types[0] = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	types[1] = goose_create_logical_type(GOOSE_TYPE_VARCHAR);

	auto status = goose_appender_create_query(tester.connection, query.c_str(), 2, types, "my_appended_data", nullptr,
	                                           &appender);
	goose_destroy_logical_type(&types[0]);
	goose_destroy_logical_type(&types[1]);
	REQUIRE(status == GooseSuccess);
	REQUIRE(goose_appender_error(appender) == nullptr);

	REQUIRE(goose_appender_begin_row(appender) == GooseSuccess);
	REQUIRE(goose_append_int32(appender, 1) == GooseSuccess);
	REQUIRE(goose_append_varchar(appender, "hello world") == GooseSuccess);
	REQUIRE(goose_appender_end_row(appender) == GooseSuccess);

	REQUIRE(goose_appender_flush(appender) == GooseSuccess);
	REQUIRE(goose_appender_close(appender) == GooseSuccess);
	REQUIRE(goose_appender_destroy(&appender) == GooseSuccess);

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info);

	// Check that the query name matches the appender query.
	auto query_name = goose_profiling_info_get_value(info, "QUERY_NAME");
	REQUIRE(query_name);
	auto query_name_c_str = goose_get_varchar(query_name);
	auto query_name_str = goose::string(query_name_c_str);
	REQUIRE(query_name_str == query);
	goose_destroy_value(&query_name);
	goose_free(query_name_c_str);

	goose::map<string, double> cumulative_counter;
	goose::map<string, double> cumulative_result;
	TraverseTree(info, cumulative_counter, cumulative_result, 0);
	tester.Cleanup();
}

TEST_CASE("Test profiling with the non-query appender", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));
	tester.Query("CREATE TABLE test (i INTEGER)");
	REQUIRE_NO_FAIL(tester.Query("PRAGMA enable_profiling = 'no_output'"));
	REQUIRE_NO_FAIL(tester.Query("SET profiling_coverage='ALL'"));

	goose_appender appender;
	REQUIRE(goose_appender_create(tester.connection, nullptr, "test", &appender) == GooseSuccess);
	REQUIRE(goose_appender_error(appender) == nullptr);

	// Appending a row.
	REQUIRE(goose_appender_begin_row(appender) == GooseSuccess);
	REQUIRE(goose_append_int32(appender, 42) == GooseSuccess);
	// Finish and flush.
	REQUIRE(goose_appender_end_row(appender) == GooseSuccess);
	REQUIRE(goose_appender_flush(appender) == GooseSuccess);
	REQUIRE(goose_appender_close(appender) == GooseSuccess);
	REQUIRE(goose_appender_destroy(&appender) == GooseSuccess);

	auto info = goose_get_profiling_info(tester.connection);
	REQUIRE(info);

	// Check that the query name matches the appender query.
	auto query_name = goose_profiling_info_get_value(info, "QUERY_NAME");
	REQUIRE(query_name);

	auto query_name_c_str = goose_get_varchar(query_name);
	auto query_name_str = goose::string(query_name_c_str);

	auto query = "INSERT INTO main.test FROM __goose_internal_appended_data";
	REQUIRE(query_name_str == query);

	goose_destroy_value(&query_name);
	goose_free(query_name_c_str);

	goose::map<string, double> cumulative_counter;
	goose::map<string, double> cumulative_result;
	TraverseTree(info, cumulative_counter, cumulative_result, 0);
	tester.Cleanup();
}
