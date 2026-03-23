// #define CATCH_CONFIG_RUNNER
#include <goose/testing/catch.h>

#include <goose/common/file_system.h>
#include <goose/common/value_operations/value_operations.h>
#include <goose/testing/compare_result.h>
#include <goose/main/query_result.h>
#include <goose/testing/test_helpers.h>
#include <goose/parser/parsed_data/copy_info.h>
#include <goose/main/client_context.h>
#include <goose/execution/operator/csv_scanner/string_value_scanner.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/testing/test_config.h>
#include "pid.h"
#include <goose/function/table/read_csv.h>
#include <goose/storage/storage_info.h>
#include <cmath>
#include <fstream>

using namespace std;

#define TESTING_DIRECTORY_NAME "goose_unittest_tempdir"

namespace goose {
static string custom_test_directory;
static case_insensitive_set_t required_requires;
static bool delete_test_path = true;

bool NO_FAIL(QueryResult &result) {
	if (result.HasError()) {
		fprintf(stderr, "Query failed with message: %s\n", result.GetError().c_str());
	}
	return !result.HasError();
}

bool NO_FAIL(goose::unique_ptr<QueryResult> result) {
	return NO_FAIL(*result);
}

void TestDeleteDirectory(string path) {
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	try {
		if (fs->DirectoryExists(path)) {
			fs->RemoveDirectory(path);
		}
	} catch (...) {
	}
}

void TestDeleteFile(string path) {
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	try {
		fs->TryRemoveFile(path);
	} catch (...) {
	}
}

void TestChangeDirectory(string path) {
	// set the base path for the tests
	FileSystem::SetWorkingDirectory(path);
}

string TestGetCurrentDirectory() {
	return FileSystem::GetWorkingDirectory();
}

void DeleteDatabase(string path) {
	if (!custom_test_directory.empty()) {
		return;
	}
	TestDeleteFile(path);
	TestDeleteFile(path + ".wal");
}

void TestCreateDirectory(string path) {
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	fs->CreateDirectory(path);
}

string TestJoinPath(string path1, string path2) {
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	return fs->JoinPath(path1, path2);
}

void SetTestDirectory(string path) {
	custom_test_directory = path;
}

void AddRequire(string require) {
	required_requires.insert(require);
}

bool IsRequired(string require) {
	return required_requires.count(require);
}

string GetTestDirectory() {
	if (custom_test_directory.empty()) {
		return TESTING_DIRECTORY_NAME;
	}
	return custom_test_directory;
}

string TestDirectoryPath() {
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	auto test_directory = GetTestDirectory();
	if (!fs->DirectoryExists(test_directory)) {
		fs->CreateDirectory(test_directory);
	}
	string path;
	if (custom_test_directory.empty()) {
		// add the PID to the test directory - but only if it was not specified explicitly by the user
		auto pid = getpid();
		path = fs->JoinPath(test_directory, to_string(pid));
	} else {
		path = test_directory;
	}
	if (!fs->DirectoryExists(path)) {
		fs->CreateDirectory(path);
	}
	return path;
}

void SetDeleteTestPath(bool delete_path) {
	delete_test_path = delete_path;
}

bool DeleteTestPath() {
	return delete_test_path;
}

void ClearTestDirectory() {
	if (!DeleteTestPath()) {
		return;
	}
	goose::unique_ptr<FileSystem> fs = FileSystem::CreateLocal();
	auto test_dir = TestDirectoryPath();
	// try to clear any files we created in the test directory
	fs->ListFiles(test_dir, [&](const string &file, bool is_dir) {
		auto full_path = fs->JoinPath(test_dir, file);
		try {
			if (is_dir) {
				fs->RemoveDirectory(full_path);
			} else {
				fs->RemoveFile(full_path);
			}
		} catch (...) {
			// skip
		}
	});
}

string TestCreatePath(string suffix) {
	return TestJoinPath(TestDirectoryPath(), suffix);
}

bool TestIsInternalError(unordered_set<string> &internal_error_messages, const string &error) {
	for (auto &error_message : internal_error_messages) {
		if (StringUtil::Contains(error, error_message)) {
			return true;
		}
	}
	return false;
}

unique_ptr<DBConfig> GetTestConfig() {
	auto &test_config = TestConfiguration::Get();

	auto result = make_uniq<DBConfig>();
#ifndef GOOSE_ALTERNATIVE_VERIFY
	result->options.checkpoint_wal_size = test_config.GetCheckpointWALSize();
	result->options.checkpoint_on_shutdown = test_config.GetCheckpointOnShutdown();
#else
	result->options.checkpoint_on_shutdown = false;
#endif
	result->options.abort_on_wal_failure = true;
#ifdef GOOSE_RUN_SLOW_VERIFIERS
	// This mode isn't slow, but we want test coverage both when it's enabled
	// and when it's not, so we enable only when GOOSE_RUN_SLOW_VERIFIERS is set.
	result->options.trim_free_blocks = true;
#endif
	result->options.allow_unsigned_extensions = true;
	auto storage_version = test_config.GetStorageVersion();
	if (!storage_version.empty()) {
		result->options.serialization_compatibility = SerializationCompatibility::FromString(storage_version);
	}

	auto max_threads = test_config.GetMaxThreads();
	if (max_threads.IsValid()) {
		result->options.maximum_threads = max_threads.GetIndex();
	}

	auto block_alloc_size = test_config.GetBlockAllocSize();
	if (block_alloc_size.IsValid()) {
		Storage::VerifyBlockAllocSize(block_alloc_size.GetIndex());
		result->options.default_block_alloc_size = block_alloc_size.GetIndex();
	}

	result->options.debug_initialize = test_config.GetDebugInitialize();
	result->options.set_variables.emplace("debug_verify_vector",
	                                      EnumUtil::ToString(test_config.GetVectorVerification()));
	return result;
}

bool CHECK_COLUMN(QueryResult &result_, size_t column_number, vector<goose::Value> values) {
	if (result_.type == QueryResultType::STREAM_RESULT) {
		fprintf(stderr, "Unexpected stream query result in CHECK_COLUMN\n");
		return false;
	}
	auto &result = (MaterializedQueryResult &)result_;
	if (result.HasError()) {
		fprintf(stderr, "Query failed with message: %s\n", result.GetError().c_str());
		return false;
	}
	if (result.names.size() != result.types.size()) {
		// column names do not match
		result.Print();
		return false;
	}
	if (values.empty()) {
		if (result.RowCount() != 0) {
			result.Print();
			return false;
		} else {
			return true;
		}
	}
	if (result.RowCount() == 0) {
		result.Print();
		return false;
	}
	if (column_number >= result.types.size()) {
		result.Print();
		return false;
	}
	for (idx_t row_idx = 0; row_idx < values.size(); row_idx++) {
		auto value = result.GetValue(column_number, row_idx);
		// NULL <> NULL, hence special handling
		if (value.IsNull() && values[row_idx].IsNull()) {
			continue;
		}

		if (!Value::DefaultValuesAreEqual(value, values[row_idx])) {
			// FAIL("Incorrect result! Got " + vector.GetValue(j).ToString()
			// +
			//      " but expected " + values[i + j].ToString());
			result.Print();
			return false;
		}
	}
	return true;
}

bool CHECK_COLUMN(goose::unique_ptr<goose::QueryResult> &result, size_t column_number, vector<goose::Value> values) {
	if (result->type == QueryResultType::STREAM_RESULT) {
		auto &stream = (StreamQueryResult &)*result;
		result = stream.Materialize();
	}
	return CHECK_COLUMN(*result, column_number, values);
}

bool CHECK_COLUMN(goose::unique_ptr<goose::MaterializedQueryResult> &result, size_t column_number,
                  vector<goose::Value> values) {
	return CHECK_COLUMN((QueryResult &)*result, column_number, values);
}

string compare_csv(goose::QueryResult &result, string csv, bool header) {
	D_ASSERT(result.type == QueryResultType::MATERIALIZED_RESULT);
	auto &materialized = (MaterializedQueryResult &)result;
	if (materialized.HasError()) {
		fprintf(stderr, "Query failed with message: %s\n", materialized.GetError().c_str());
		return materialized.GetError();
	}
	string error;
	if (!compare_result(csv, materialized.Collection(), materialized.types, header, error)) {
		return error;
	}
	return "";
}

string compare_csv_collection(goose::ColumnDataCollection &collection, string csv, bool header) {
	string error;
	if (!compare_result(csv, collection, collection.Types(), header, error)) {
		return error;
	}
	return "";
}

string show_diff(DataChunk &left, DataChunk &right) {
	if (left.ColumnCount() != right.ColumnCount()) {
		return StringUtil::Format("Different column counts: %d vs %d", (int)left.ColumnCount(),
		                          (int)right.ColumnCount());
	}
	if (left.size() != right.size()) {
		return StringUtil::Format("Different sizes: %zu vs %zu", left.size(), right.size());
	}
	string difference;
	for (size_t i = 0; i < left.ColumnCount(); i++) {
		bool has_differences = false;
		auto &left_vector = left.data[i];
		auto &right_vector = right.data[i];
		string left_column = StringUtil::Format("Result\n------\n%s [", left_vector.GetType().ToString().c_str());
		string right_column = StringUtil::Format("Expect\n------\n%s [", right_vector.GetType().ToString().c_str());
		if (left_vector.GetType() == right_vector.GetType()) {
			for (size_t j = 0; j < left.size(); j++) {
				auto left_value = left_vector.GetValue(j);
				auto right_value = right_vector.GetValue(j);
				if (!Value::DefaultValuesAreEqual(left_value, right_value)) {
					left_column += left_value.ToString() + ",";
					right_column += right_value.ToString() + ",";
					has_differences = true;
				} else {
					left_column += "_,";
					right_column += "_,";
				}
			}
		} else {
			left_column += "...";
			right_column += "...";
		}
		left_column += "]\n";
		right_column += "]\n";
		if (has_differences) {
			difference += StringUtil::Format("Difference in column %d:\n", i);
			difference += left_column + "\n" + right_column + "\n";
		}
	}
	return difference;
}

//! Compares the result of a pipe-delimited CSV with the given DataChunk
//! Returns true if they are equal, and stores an error_message otherwise
bool compare_result(string csv, ColumnDataCollection &collection, vector<LogicalType> sql_types, bool has_header,
                    string &error_message) {
	D_ASSERT(collection.Count() == 0 || collection.Types().size() == sql_types.size());

	// create the csv on disk
	auto csv_path = TestCreatePath("__test_csv_path.csv");
	ofstream f(csv_path);
	f << csv;
	f.close();

	// set up the CSV reader
	CSVReaderOptions options;
	options.auto_detect = false;
	options.dialect_options.state_machine_options.delimiter = {"|"};
	options.dialect_options.header = has_header;
	options.dialect_options.state_machine_options.quote = '\"';
	options.dialect_options.state_machine_options.escape = '\"';
	options.file_path = csv_path;
	options.dialect_options.num_cols = sql_types.size();
	// set up the intermediate result chunk
	DataChunk parsed_result;
	parsed_result.Initialize(Allocator::DefaultAllocator(), sql_types);

	Goose db;
	Connection con(db);
	MultiFileOptions file_options;
	auto scanner_ptr = StringValueScanner::GetCSVScanner(*con.context, options, file_options);
	auto &scanner = *scanner_ptr;
	ColumnDataCollection csv_data_collection(*con.context, sql_types);
	while (!scanner.FinishedIterator()) {
		// parse a chunk from the CSV file
		try {
			parsed_result.Reset();
			scanner.Flush(parsed_result);
		} catch (std::exception &ex) {
			error_message = "Could not parse CSV: " + string(ex.what());
			return false;
		}
		if (parsed_result.size() == 0) {
			break;
		}
		csv_data_collection.Append(parsed_result);
	}
	string error;
	if (!ColumnDataCollection::ResultEquals(collection, csv_data_collection, error_message)) {
		return false;
	}
	return true;
}

} // namespace goose
