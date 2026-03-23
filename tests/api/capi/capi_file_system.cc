#include "capi_tester.h"

using namespace goose;
using namespace std;

static void test_file_system(goose_file_system fs, string file_name) {
	REQUIRE(fs != nullptr);

	goose_state state = GooseSuccess;
	goose_file_handle file;

	auto file_path = TestDirectoryPath() + "/" + file_name;

	auto options = goose_create_file_open_options();
	state = goose_file_open_options_set_flag(options, GOOSE_FILE_FLAG_WRITE, true);
	REQUIRE(state == GooseSuccess);
	state = goose_file_open_options_set_flag(options, GOOSE_FILE_FLAG_READ, true);
	REQUIRE(state == GooseSuccess);

	// Try to open non-existing file without create flag
	state = goose_file_system_open(fs, file_path.c_str(), options, &file);
	REQUIRE(state != GooseSuccess);
	auto error_data = goose_file_system_error_data(fs);
	auto error_type = goose_error_data_error_type(error_data);
	REQUIRE(error_type == GOOSE_ERROR_IO);
	goose_destroy_error_data(&error_data);

	// Try to open file without options
	state = goose_file_system_open(fs, file_path.c_str(), nullptr, &file);
	REQUIRE(state == GooseError);
	error_data = goose_file_system_error_data(fs);
	error_type = goose_error_data_error_type(error_data);
	REQUIRE(error_type == GOOSE_ERROR_IO);
	goose_destroy_error_data(&error_data);

	// Try to open incorrect file
	state = goose_file_system_open(fs, file_path.c_str(), options, nullptr);
	REQUIRE(state == GooseError);
	error_data = goose_file_system_error_data(fs);
	error_type = goose_error_data_error_type(error_data);
	REQUIRE(error_type == GOOSE_ERROR_IO);
	goose_destroy_error_data(&error_data);

	// Try to open file with incorrect filename
	state = goose_file_system_open(fs, nullptr, options, &file);
	REQUIRE(state == GooseError);
	error_data = goose_file_system_error_data(fs);
	error_type = goose_error_data_error_type(error_data);
	REQUIRE(error_type == GOOSE_ERROR_IO);
	goose_destroy_error_data(&error_data);

	// Try to open with all incorrect input parameters
	// fs is incorrect, error message verification is not possible
	state = goose_file_system_open(nullptr, nullptr, nullptr, nullptr);
	REQUIRE(state == GooseError);

	// Try to write to a null file handle
	auto failed_bytes_written = goose_file_handle_write(file, "data", 4);
	REQUIRE(failed_bytes_written == -1);
	auto file_error_data = goose_file_handle_error_data(file);
	auto has_error = goose_error_data_has_error(file_error_data);
	REQUIRE(has_error == false);
	goose_destroy_error_data(&file_error_data);

	// Set create flag
	state = goose_file_open_options_set_flag(options, GOOSE_FILE_FLAG_CREATE, true);
	REQUIRE(state == GooseSuccess);

	// Create and open a file
	state = goose_file_system_open(fs, file_path.c_str(), options, &file);
	REQUIRE(state == GooseSuccess);
	REQUIRE(file != nullptr);

	// Write to the file
	const char *data = "Hello, Goose File System!";
	auto bytes_written = goose_file_handle_write(file, data, strlen(data));
	REQUIRE(bytes_written == (int64_t)strlen(data));
	auto position = goose_file_handle_tell(file);
	REQUIRE(position == bytes_written);
	auto size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);

	// Sync
	state = goose_file_handle_sync(file);

	// Seek to the beginning
	state = goose_file_handle_seek(file, 0);
	REQUIRE(state == GooseSuccess);
	position = goose_file_handle_tell(file);
	REQUIRE(position == 0);

	// Read from the file
	char buffer[30];
	memset(buffer, 0, sizeof(buffer));
	auto bytes_read = goose_file_handle_read(file, buffer, sizeof(buffer) - 1);
	REQUIRE(bytes_read == bytes_written);
	REQUIRE(strcmp(buffer, data) == 0);
	position = goose_file_handle_tell(file);
	REQUIRE(position == bytes_read);
	size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);

	// Seek to the end
	state = goose_file_handle_seek(file, bytes_written);
	REQUIRE(state == GooseSuccess);
	position = goose_file_handle_tell(file);
	REQUIRE(position == bytes_written);
	size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);

	// Try to read from the end of the file
	memset(buffer, 0, sizeof(buffer));
	bytes_read = goose_file_handle_read(file, buffer, sizeof(buffer) - 1);
	REQUIRE(bytes_read == 0); // EOF
	position = goose_file_handle_tell(file);
	REQUIRE(position == bytes_written);
	size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);

	// Seek back to the beginning
	state = goose_file_handle_seek(file, 0);
	REQUIRE(state == GooseSuccess);
	position = goose_file_handle_tell(file);
	REQUIRE(position == 0);
	size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);

	// Close the file
	goose_file_handle_close(file);
	goose_destroy_file_handle(&file);

	// Open file again for reading
	state = goose_file_system_open(fs, file_path.c_str(), options, &file);
	REQUIRE(state == GooseSuccess);
	REQUIRE(file != nullptr);
	size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);
	// Check that the data is still there
	memset(buffer, 0, sizeof(buffer));
	bytes_read = goose_file_handle_read(file, buffer, sizeof(buffer) - 1);
	REQUIRE(bytes_read == bytes_written);
	REQUIRE(strcmp(buffer, data) == 0);
	position = goose_file_handle_tell(file);
	REQUIRE(position == bytes_read);
	size = goose_file_handle_size(file);
	REQUIRE(size == bytes_written);

	// Close the file again
	goose_file_handle_close(file);
	goose_destroy_file_handle(&file);

	goose_destroy_file_open_options(&options);

	REQUIRE(file == nullptr);
	REQUIRE(options == nullptr);

	// Try destroy again for good measure
	goose_destroy_file_handle(&file);
	goose_destroy_file_open_options(&options);

	REQUIRE(file == nullptr);
	REQUIRE(options == nullptr);
}

TEST_CASE("Test File System in C API", "[capi]") {
	CAPITester tester;
	goose_client_context context;
	goose_file_system fs;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));

	// get a file system from the client context
	goose_connection_get_client_context(tester.connection, &context);
	fs = goose_client_context_get_file_system(context);
	REQUIRE(fs != nullptr);

	test_file_system(fs, "test_file_capi_1.txt");

	goose_destroy_file_system(&fs);
	REQUIRE(fs == nullptr);

	goose_destroy_client_context(&context);

	// Try to destory fs again
	goose_destroy_file_system(&fs);
	REQUIRE(fs == nullptr);
}
