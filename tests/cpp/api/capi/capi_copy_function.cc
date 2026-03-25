#include <goose/testing/capi_tester.h>
#include <goose/goose-c.h>

#include <cstring> // for strcmp

using namespace goose;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------
// COPY (...) TO (...)
//----------------------------------------------------------------------------------------------------------------------

struct MyCopyFunctionExtraInfo {
	idx_t illegal_min_value = 42;
};

struct MyCopyFunctionBindData {
	idx_t max_size;
	idx_t min_size;
};

struct MyCopyFunctionGlobalState {
	idx_t total_written_bytes = 0;
	goose_file_system file_system = nullptr;
	goose_file_handle file_handle = nullptr;

	~MyCopyFunctionGlobalState() {
		if (file_handle) {
			goose_destroy_file_handle(&file_handle);
		}
		if (file_system) {
			goose_destroy_file_system(&file_system);
		}
	}
};

static void MyCopyFunctionBind(goose_copy_function_bind_info info) {
	// COVERAGE
	goose_copy_function_bind_set_error(nullptr, "foo");
	goose_copy_function_bind_set_error(info, nullptr);
	goose_copy_function_bind_set_error(nullptr, nullptr);
	REQUIRE(goose_copy_function_bind_get_column_count(nullptr) == 0);
	REQUIRE(goose_copy_function_bind_get_column_type(nullptr, 0) == nullptr);
	REQUIRE(goose_copy_function_bind_get_column_type(info, 9999) == nullptr);
	REQUIRE(goose_copy_function_bind_get_options(nullptr) == nullptr);
	REQUIRE(goose_copy_function_bind_get_client_context(nullptr) == nullptr);
	REQUIRE(goose_copy_function_bind_get_extra_info(nullptr) == nullptr);

	auto options = goose_copy_function_bind_get_options(info);
	if (!options) {
		goose_copy_function_bind_set_error(info, "No options given!");
		return;
	}

	// Extract options
	auto options_type = goose_get_value_type(options);
	if (goose_get_type_id(options_type) != GOOSE_TYPE_STRUCT) {
		goose_destroy_value(&options);
		goose_copy_function_bind_set_error(info, "No options given!");
		return;
	}
	auto struct_size = goose_struct_type_child_count(options_type);
	if (struct_size > 2) {
		goose_destroy_value(&options);
		goose_copy_function_bind_set_error(info, "Too many options given!");
		return;
	}

	// Extract max_size and min_size
	int32_t min_size = 0;
	int32_t max_size = 0;

	for (idx_t i = 0; i < struct_size; i++) {
		auto child_name = goose_struct_type_child_name(options_type, i);
		auto child_value = goose_get_struct_child(options, i);
		auto child_type = goose_get_value_type(child_value);
		if (goose_get_type_id(child_type) != GOOSE_TYPE_INTEGER) {
			goose_destroy_value(&options);
			goose_destroy_value(&child_value);
			goose_free(child_name);
			goose_copy_function_bind_set_error(info, "Options must be of type INT");
			return;
		}
		if (strcmp(child_name, "MAX_SIZE") == 0) {
			max_size = goose_get_int32(child_value);
		} else if (strcmp(child_name, "MIN_SIZE") == 0) {
			min_size = goose_get_int32(child_value);
		} else {
			goose_destroy_value(&options);
			goose_destroy_value(&child_value);
			goose_free(child_name);
			goose_copy_function_bind_set_error(info, "Unknown option given");
			return;
		}
		goose_free(child_name);
		goose_destroy_value(&child_value);
	}

	if (max_size < 0) {
		goose_destroy_value(&options);
		goose_copy_function_bind_set_error(info, "MAX_SIZE must be >= 0");
		return;
	}
	if (min_size < 0) {
		goose_destroy_value(&options);
		goose_copy_function_bind_set_error(info, "MIN_SIZE must be >= 0");
		return;
	}

	// Now were done with options, destroy it!
	goose_destroy_value(&options);

	// Now inspect the input columns
	auto column_count = goose_copy_function_bind_get_column_count(info);
	if (column_count != 1) {
		goose_copy_function_bind_set_error(info, "Expected exactly one column");
		return;
	}

	auto column_type = goose_copy_function_bind_get_column_type(info, 0);
	if (goose_get_type_id(column_type) != GOOSE_TYPE_BIGINT) {
		goose_copy_function_bind_set_error(info, "Expected column of type BIGINT");
		goose_destroy_logical_type(&column_type);
		return;
	}

	auto my_bind_data = new MyCopyFunctionBindData();
	my_bind_data->max_size = max_size;
	my_bind_data->min_size = min_size;

	goose_copy_function_bind_set_bind_data(info, my_bind_data, [](void *bind_data) {
		auto my_bind_data = (MyCopyFunctionBindData *)bind_data;
		delete my_bind_data;
	});

	goose_destroy_value(&options);
	goose_destroy_logical_type(&column_type);
}

static void MyCopyFunctionInit(goose_copy_function_global_init_info info) {
	// COVERAGE
	goose_copy_function_global_init_set_error(nullptr, "foo");
	goose_copy_function_global_init_set_error(info, nullptr);
	goose_copy_function_global_init_set_error(nullptr, nullptr);
	REQUIRE(goose_copy_function_global_init_get_client_context(nullptr) == nullptr);
	REQUIRE(goose_copy_function_global_init_get_extra_info(nullptr) == nullptr);
	REQUIRE(goose_copy_function_global_init_get_bind_data(nullptr) == nullptr);
	REQUIRE(goose_copy_function_global_init_get_file_path(nullptr) == nullptr);
	goose_copy_function_global_init_set_global_state(nullptr, nullptr, nullptr);

	auto bind_data = (MyCopyFunctionBindData *)goose_copy_function_global_init_get_bind_data(info);
	auto extra_info = (MyCopyFunctionExtraInfo *)goose_copy_function_global_init_get_extra_info(info);
	auto client_context = goose_copy_function_global_init_get_client_context(info);

	if (bind_data->min_size == extra_info->illegal_min_value) {
		// Ooops, forgot to check this in the bind!
		goose_copy_function_global_init_set_error(info, "My bad, min_size cannot be set to that value!");
		goose_destroy_client_context(&client_context);
		return;
	}

	// Initialize state
	auto g_state = new MyCopyFunctionGlobalState();
	goose_copy_function_global_init_set_global_state(info, g_state, [](void *state) {
		auto g_state = (MyCopyFunctionGlobalState *)state;
		delete g_state;
	});

	// Setup file system and open the file
	g_state->total_written_bytes = 0;
	g_state->file_system = goose_client_context_get_file_system(client_context);

	auto file_path = goose_copy_function_global_init_get_file_path(info);
	auto file_flag = goose_create_file_open_options();
	goose_file_open_options_set_flag(file_flag, GOOSE_FILE_FLAG_WRITE, true);
	goose_file_open_options_set_flag(file_flag, GOOSE_FILE_FLAG_CREATE, true);

	if (goose_file_system_open(g_state->file_system, file_path, file_flag, &g_state->file_handle) != GooseSuccess) {
		auto error_data = goose_file_system_error_data(g_state->file_system);
		goose_copy_function_global_init_set_error(info, goose_error_data_message(error_data));
		goose_destroy_error_data(&error_data);
	}

	goose_destroy_file_open_options(&file_flag);
	goose_destroy_client_context(&client_context);
}

static void MyCopyFunctionSink(goose_copy_function_sink_info info, goose_data_chunk input) {
	// COVERAGE
	goose_copy_function_sink_set_error(nullptr, "foo");
	goose_copy_function_sink_set_error(info, nullptr);
	goose_copy_function_sink_set_error(nullptr, nullptr);
	REQUIRE(goose_copy_function_sink_get_client_context(nullptr) == nullptr);
	REQUIRE(goose_copy_function_sink_get_extra_info(nullptr) == nullptr);
	REQUIRE(goose_copy_function_sink_get_bind_data(nullptr) == nullptr);
	REQUIRE(goose_copy_function_sink_get_global_state(nullptr) == nullptr);

	auto bind_data = (MyCopyFunctionBindData *)goose_copy_function_sink_get_bind_data(info);
	auto g_state = (MyCopyFunctionGlobalState *)goose_copy_function_sink_get_global_state(info);

	// Sink the data
	auto row_count = goose_data_chunk_get_size(input);
	auto col_vec = goose_data_chunk_get_vector(input, 0);
	auto col_data = (int64_t *)goose_vector_get_data(col_vec);

	for (idx_t r = 0; r < row_count; r++) {
		auto written = goose_file_handle_write(g_state->file_handle, &col_data[r], sizeof(int64_t));
		if (written != sizeof(int64_t)) {
			auto error_data = goose_file_handle_error_data(g_state->file_handle);
			goose_copy_function_sink_set_error(info, goose_error_data_message(error_data));
			goose_destroy_error_data(&error_data);
			return;
		}
		g_state->total_written_bytes += written;

		if (g_state->total_written_bytes > bind_data->max_size) {
			goose_copy_function_sink_set_error(info, "Wrote too much data");
			return;
		}
	}
}

static void MyCopyFunctionFinalize(goose_copy_function_finalize_info info) {
	// COVERAGE
	goose_copy_function_finalize_set_error(nullptr, "foo");
	goose_copy_function_finalize_set_error(info, nullptr);
	goose_copy_function_finalize_set_error(nullptr, nullptr);
	REQUIRE(goose_copy_function_finalize_get_client_context(nullptr) == nullptr);
	REQUIRE(goose_copy_function_finalize_get_extra_info(nullptr) == nullptr);
	REQUIRE(goose_copy_function_finalize_get_bind_data(nullptr) == nullptr);
	REQUIRE(goose_copy_function_finalize_get_global_state(nullptr) == nullptr);

	auto bind_data = (MyCopyFunctionBindData *)goose_copy_function_finalize_get_bind_data(info);
	auto g_state = (MyCopyFunctionGlobalState *)goose_copy_function_finalize_get_global_state(info);

	// Check that we actually wrote enough!
	if (g_state->total_written_bytes < bind_data->min_size) {
		goose_copy_function_finalize_set_error(info, "Wrote too little data");
		return;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// COPY (...) FROM (...)
//----------------------------------------------------------------------------------------------------------------------
struct MyCopyFromFunctionBindData {
	string file_path;
	int32_t max_size = 0;
	int32_t min_size = 0;
	goose_client_context client_context = nullptr;

	~MyCopyFromFunctionBindData() {
		if (client_context) {
			goose_destroy_client_context(&client_context);
		}
	}
};

struct MyCopyFromFunctionState {
	goose_file_system file_system = nullptr;
	goose_file_handle file_handle = nullptr;
	int64_t total_read_bytes = 0;

	~MyCopyFromFunctionState() {
		if (file_handle) {
			goose_destroy_file_handle(&file_handle);
		}
		if (file_system) {
			goose_destroy_file_system(&file_system);
		}
	}
};

static void MyCopyFromFunctionBind(goose_bind_info info) {
	// COVERAGE
	REQUIRE(goose_table_function_bind_get_result_column_count(nullptr) == 0);
	REQUIRE(goose_table_function_bind_get_result_column_name(nullptr, 0) == nullptr);
	REQUIRE(goose_table_function_bind_get_result_column_name(info, 9999) == nullptr);
	REQUIRE(goose_table_function_bind_get_result_column_type(nullptr, 0) == nullptr);
	REQUIRE(goose_table_function_bind_get_result_column_type(info, 9999) == nullptr);

	// Ensure we have exactly one expected column of type BIGINT
	auto result_column_count = goose_table_function_bind_get_result_column_count(info);
	if (result_column_count != 1) {
		auto error_msg = "MY_COPY: Expected exactly one target column!";
		goose_bind_set_error(info, error_msg);
		return;
	}

	for (idx_t i = 0; i < result_column_count; i++) {
		auto col_type = goose_table_function_bind_get_result_column_type(info, i);
		if (goose_get_type_id(col_type) != GOOSE_TYPE_BIGINT) {
			auto col_name = goose_table_function_bind_get_result_column_name(info, i);
			auto error_msg = StringUtil::Format("MY_COPY: Target column '%s' is not of type BIGINT!", col_name);
			goose_bind_set_error(info, error_msg.c_str());
			goose_destroy_logical_type(&col_type);
			return;
		}
		goose_destroy_logical_type(&col_type);
	}

	auto file_path = goose_bind_get_parameter(info, 0);
	auto max_size = goose_bind_get_named_parameter(info, "MAX_SIZE");
	auto min_size = goose_bind_get_named_parameter(info, "MIN_SIZE");

	if (!file_path || !max_size || !min_size) {
		goose_destroy_value(&file_path);
		goose_destroy_value(&max_size);
		goose_destroy_value(&min_size);
		goose_bind_set_error(info, "MY_COPY: Error retrieving parameters!");
		return;
	}

	auto bind_data = new MyCopyFromFunctionBindData();

	// Get and set parameters
	auto file_path_str = goose_get_varchar(file_path);
	bind_data->file_path = string(file_path_str);
	goose_free(file_path_str);

	bind_data->max_size = goose_get_int32(max_size);
	bind_data->min_size = goose_get_int32(min_size);

	goose_destroy_value(&file_path);
	goose_destroy_value(&max_size);
	goose_destroy_value(&min_size);

	// Also get file client context
	goose_table_function_get_client_context(info, &bind_data->client_context);

	goose_bind_set_bind_data(info, bind_data, [](void *bind_data) {
		auto my_bind_data = (MyCopyFromFunctionBindData *)bind_data;
		delete my_bind_data;
	});
}

static void MyCopyFromFunctionInit(goose_init_info info) {
	auto bind_data = (MyCopyFromFunctionBindData *)goose_init_get_bind_data(info);
	auto state = new MyCopyFromFunctionState();

	auto client_context = bind_data->client_context;
	state->file_system = goose_client_context_get_file_system(client_context);
	auto file_flag = goose_create_file_open_options();
	goose_file_open_options_set_flag(file_flag, GOOSE_FILE_FLAG_READ, true);

	auto ok = goose_file_system_open(state->file_system, bind_data->file_path.c_str(), file_flag, &state->file_handle);
	if (ok == GooseError) {
		auto error_data = goose_file_system_error_data(state->file_system);
		goose_init_set_error(info, goose_error_data_message(error_data));
		goose_destroy_error_data(&error_data);
		goose_destroy_file_open_options(&file_flag);
		delete state;
		return;
	}

	goose_destroy_file_open_options(&file_flag);
	state->total_read_bytes = 0;

	goose_init_set_init_data(info, state, [](void *state) {
		auto my_state = (MyCopyFromFunctionState *)state;
		delete my_state;
	});
}

static void MyCopyFromFunction(goose_function_info info, goose_data_chunk output) {
	// Now read data from file
	auto bind_data = (MyCopyFromFunctionBindData *)goose_function_get_bind_data(info);
	auto state = (MyCopyFromFunctionState *)goose_function_get_init_data(info);

	auto col_vec = goose_data_chunk_get_vector(output, 0);
	auto col_data = (int64_t *)goose_vector_get_data(col_vec);

	idx_t read_count = 0;
	for (read_count = 0; read_count < STANDARD_VECTOR_SIZE; read_count++) {
		int64_t read_value;
		auto read_bytes = goose_file_handle_read(state->file_handle, &read_value, sizeof(int64_t));
		if (read_bytes == 0) {
			// EOF
			if (state->total_read_bytes < bind_data->min_size) {
				goose_function_set_error(info, "Read too little data");
				return;
			}
			break;
		}

		if (read_bytes < 0) {
			auto error_data = goose_file_handle_error_data(state->file_handle);
			goose_function_set_error(info, goose_error_data_message(error_data));
			goose_destroy_error_data(&error_data);
			return;
		}

		if (read_bytes != sizeof(int64_t)) {
			goose_function_set_error(info, "Could not read full value");
			return;
		}

		col_data[read_count] = read_value;
		state->total_read_bytes += read_bytes;

		if (state->total_read_bytes > bind_data->max_size) {
			goose_function_set_error(info, "Read too much data");
			return;
		}
	}

	// Set the output count
	goose_data_chunk_set_size(output, read_count);
}

//----------------------------------------------------------------------------------------------------------------------
// Register
//----------------------------------------------------------------------------------------------------------------------

TEST_CASE("Test Copy Functions in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));

	goose_copy_function func = goose_create_copy_function();
	REQUIRE(func != nullptr);

	// Set my extra info
	auto my_extra_info = new MyCopyFunctionExtraInfo();
	my_extra_info->illegal_min_value = 42;
	goose_copy_function_set_extra_info(func, my_extra_info, [](void *data) {
		auto my_extra_info = (MyCopyFunctionExtraInfo *)data;
		delete my_extra_info;
	});

	// Need to have a name
	auto status = goose_register_copy_function(tester.connection, func);
	REQUIRE(status == GooseError);

	// Set name
	goose_copy_function_set_name(func, "my_copy");

	status = goose_register_copy_function(tester.connection, func);
	REQUIRE(status == GooseError);

	// Need to have function pointers set
	goose_copy_function_set_bind(func, MyCopyFunctionBind);
	goose_copy_function_set_global_init(func, MyCopyFunctionInit);
	goose_copy_function_set_sink(func, MyCopyFunctionSink);
	goose_copy_function_set_finalize(func, MyCopyFunctionFinalize);

	// Also add a scan function
	auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);

	auto scan_func = goose_create_table_function();
	goose_table_function_add_parameter(scan_func, varchar_type);
	goose_table_function_add_named_parameter(scan_func, "MAX_SIZE", int_type);
	goose_table_function_add_named_parameter(scan_func, "MIN_SIZE", int_type);
	goose_table_function_set_bind(scan_func, MyCopyFromFunctionBind);
	goose_table_function_set_init(scan_func, MyCopyFromFunctionInit);
	goose_table_function_set_function(scan_func, MyCopyFromFunction);
	goose_table_function_set_name(scan_func, "my_copy");

	goose_destroy_logical_type(&varchar_type);
	goose_destroy_logical_type(&int_type);

	goose_copy_function_set_copy_from_function(func, scan_func);

	goose_destroy_table_function(&scan_func);

	status = goose_register_copy_function(tester.connection, func);
	REQUIRE(status == GooseSuccess);

	auto file_path = TestDirectoryPath() + "/" + "test_copy";

	// Try write too little
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(10) as r(i)) TO '%s1.txt' (FORMAT MY_COPY, MIN_SIZE 2000, MAX_SIZE 1000)",
	    file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Wrote too little data"));

	// Try write too much
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(10) as r(i)) TO '%s2.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 5)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Wrote too much data"));

	// Try write some non-int data
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i::VARCHAR FROM range(10) as r(i)) TO '%s3.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 100)",
	    file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Expected column of type BIGINT"));

	// Try write some data with illegal min-size
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(3) as r(i)) TO '%s4.txt' (FORMAT MY_COPY, MIN_SIZE 42, MAX_SIZE 100)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "My bad, min_size cannot be set to that value!"));

	// Try write with unknown option
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(10) as r(i)) TO '%s5.txt' (FORMAT MY_COPY, MIN_SIZE 0, UNKNOWN 5)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Unknown option given"));

	// Try write with too many options
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(10) as r(i)) TO '%s6.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 100, EXTRA 5)",
	    file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Too many options given"));

	// Try write with a non-int option
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(10) as r(i)) TO '%s7.txt' (FORMAT MY_COPY, MIN_SIZE 'hello', MAX_SIZE 100)",
	    file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Options must be of type INT"));

	// Try write just right
	result = tester.Query(StringUtil::Format(
	    "COPY (SELECT i FROM range(10) as r(i)) TO '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 100)", file_path));
	REQUIRE_NO_FAIL(*result);

	//----------------------------------
	// Now read with COPY ... FROM (...)
	//----------------------------------

	// Now try to read it back in
	result = tester.Query("CREATE TABLE my_table(i BIGINT)");
	REQUIRE_NO_FAIL(*result);

	// Read non-existing file
	result = tester.Query("COPY my_table FROM 'non_existing_file.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 100)");
	REQUIRE_FAIL(result);

	// Read with too small max size
	result = tester.Query(
	    StringUtil::Format("COPY my_table FROM '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 10)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Read too much data"));

	// Read with too large min size
	result = tester.Query(
	    StringUtil::Format("COPY my_table FROM '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 1000, MAX_SIZE 10000)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Read too little data"));

	// Read with non-int target column
	result = tester.Query("CREATE TABLE my_varchar_table(i VARCHAR)");
	REQUIRE_NO_FAIL(*result);
	result = tester.Query(StringUtil::Format(
	    "COPY my_varchar_table FROM '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 10000)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Target column 'i' is not of type BIGINT!"));
	result = tester.Query("DROP TABLE my_varchar_table");
	REQUIRE_NO_FAIL(*result);

	// Read with two target columns
	result = tester.Query("CREATE TABLE my_two_col_table(i BIGINT, j BIGINT)");
	REQUIRE_NO_FAIL(*result);
	result = tester.Query(StringUtil::Format(
	    "COPY my_two_col_table FROM '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 10000)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "Expected exactly one target column!"));
	result = tester.Query("DROP TABLE my_two_col_table");
	REQUIRE_NO_FAIL(*result);

	// Read with unknown option
	result = tester.Query(StringUtil::Format(
	    "COPY my_table FROM '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 10000, UNKNOWN 5)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(),
	                             "'UNKNOWN' is not a supported option for copy function 'my_copy'"));

	// Read with missing option
	result =
	    tester.Query(StringUtil::Format("COPY my_table FROM '%s8.txt' (FORMAT MY_COPY, MAX_SIZE 10000)", file_path));
	REQUIRE_FAIL(result);
	REQUIRE(StringUtil::Contains(result->ErrorMessage(), "MY_COPY: Error retrieving parameters!"));

	// Read just right
	result = tester.Query(
	    StringUtil::Format("COPY my_table FROM '%s8.txt' (FORMAT MY_COPY, MIN_SIZE 0, MAX_SIZE 10000)", file_path));
	REQUIRE_NO_FAIL(*result);
	result = tester.Query("SELECT COUNT(*) FROM my_table");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<int64_t>(0, 0) == 10);
	result = tester.Query("SELECT SUM(i) FROM my_table");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<int64_t>(0, 0) == 45);

	// Destroy
	goose_destroy_copy_function(&func);
	goose_destroy_copy_function(&func);
}
