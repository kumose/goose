#include "capi_tester.h"
#include <goose/common/arrow/arrow_appender.h>
#include <goose/common/arrow/arrow_converter.h>

using namespace goose;
using namespace std;

TEST_CASE("Test arrow in C API", "[capi][arrow]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	goose_prepared_statement stmt = nullptr;
	goose_arrow arrow_result = nullptr;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	SECTION("test rows changed") {
		REQUIRE_NO_FAIL(tester.Query("CREATE TABLE test(a INTEGER);"));
		auto state = goose_query_arrow(tester.connection, "INSERT INTO test VALUES (1), (2);", &arrow_result);
		REQUIRE(state == GooseSuccess);
		REQUIRE(goose_arrow_rows_changed(arrow_result) == 2);
		goose_destroy_arrow(&arrow_result);
		REQUIRE_NO_FAIL(tester.Query("DROP TABLE test;"));
	}

	SECTION("test query arrow") {
		auto state = goose_query_arrow(tester.connection, "SELECT 42 AS VALUE, [1,2,3,4,5] AS LST;", &arrow_result);
		REQUIRE(state == GooseSuccess);
		REQUIRE(goose_arrow_row_count(arrow_result) == 1);
		REQUIRE(goose_arrow_column_count(arrow_result) == 2);
		REQUIRE(goose_arrow_rows_changed(arrow_result) == 0);

		// query the arrow schema
		ArrowSchema arrow_schema;
		arrow_schema.Init();
		auto arrow_schema_ptr = &arrow_schema;

		state = goose_query_arrow_schema(arrow_result, reinterpret_cast<goose_arrow_schema *>(&arrow_schema_ptr));
		REQUIRE(state == GooseSuccess);
		REQUIRE(string(arrow_schema.name) == "goose_query_result");
		if (arrow_schema.release) {
			arrow_schema.release(arrow_schema_ptr);
		}

		// query array data
		ArrowArray arrow_array;
		arrow_array.Init();
		auto arrow_array_ptr = &arrow_array;

		state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&arrow_array_ptr));
		REQUIRE(state == GooseSuccess);
		REQUIRE(arrow_array.length == 1);
		REQUIRE(arrow_array.release != nullptr);
		arrow_array.release(arrow_array_ptr);

		goose_arrow_array null_array = nullptr;
		state = goose_query_arrow_array(arrow_result, &null_array);
		REQUIRE(state == GooseSuccess);
		REQUIRE(null_array == nullptr);

		// destroy the arrow result
		goose_destroy_arrow(&arrow_result);
	}

	SECTION("test multiple chunks") {
		// create a table that consists of multiple chunks
		REQUIRE_NO_FAIL(tester.Query("CREATE TABLE test(a INTEGER);"));
		REQUIRE_NO_FAIL(
		    tester.Query("INSERT INTO test SELECT i FROM (VALUES (1), (2), (3), (4), (5)) t(i), range(500);"));
		auto state = goose_query_arrow(tester.connection, "SELECT CAST(a AS INTEGER) AS a FROM test ORDER BY a;",
		                                &arrow_result);
		REQUIRE(state == GooseSuccess);

		// query the arrow schema
		ArrowSchema arrow_schema;
		arrow_schema.Init();
		auto arrow_schema_ptr = &arrow_schema;

		state = goose_query_arrow_schema(arrow_result, reinterpret_cast<goose_arrow_schema *>(&arrow_schema_ptr));
		REQUIRE(state == GooseSuccess);
		REQUIRE(arrow_schema.release != nullptr);
		arrow_schema.release(arrow_schema_ptr);

		int total_count = 0;
		while (true) {
			// query array data
			ArrowArray arrow_array;
			arrow_array.Init();
			auto arrow_array_ptr = &arrow_array;

			state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&arrow_array_ptr));
			REQUIRE(state == GooseSuccess);

			if (arrow_array.length == 0) {
				// nothing to release
				REQUIRE(total_count == 2500);
				break;
			}

			REQUIRE(arrow_array.length > 0);
			total_count += arrow_array.length;
			REQUIRE(arrow_array.release != nullptr);
			arrow_array.release(arrow_array_ptr);
		}

		// destroy the arrow result
		goose_destroy_arrow(&arrow_result);
		REQUIRE_NO_FAIL(tester.Query("DROP TABLE test;"));
	}

	SECTION("test prepare query arrow") {
		auto state = goose_prepare(tester.connection, "SELECT CAST($1 AS BIGINT)", &stmt);
		REQUIRE(state == GooseSuccess);
		REQUIRE(stmt != nullptr);
		REQUIRE(goose_bind_int64(stmt, 1, 42) == GooseSuccess);

		// prepare and execute the arrow schema
		ArrowSchema prepared_schema;
		prepared_schema.Init();
		auto prepared_schema_ptr = &prepared_schema;

		state = goose_prepared_arrow_schema(stmt, reinterpret_cast<goose_arrow_schema *>(&prepared_schema_ptr));
		REQUIRE(state == GooseSuccess);
		REQUIRE(string(prepared_schema.format) == "+s");
		REQUIRE(goose_execute_prepared_arrow(stmt, nullptr) == GooseError);
		REQUIRE(goose_execute_prepared_arrow(stmt, &arrow_result) == GooseSuccess);
		REQUIRE(prepared_schema.release != nullptr);
		prepared_schema.release(prepared_schema_ptr);

		// query the arrow schema
		ArrowSchema arrow_schema;
		arrow_schema.Init();
		auto arrow_schema_ptr = &arrow_schema;

		state = goose_query_arrow_schema(arrow_result, reinterpret_cast<goose_arrow_schema *>(&arrow_schema_ptr));
		REQUIRE(state == GooseSuccess);
		REQUIRE(string(arrow_schema.format) == "+s");
		REQUIRE(arrow_schema.release != nullptr);
		arrow_schema.release(arrow_schema_ptr);

		ArrowArray arrow_array;
		arrow_array.Init();
		auto arrow_array_ptr = &arrow_array;

		state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&arrow_array_ptr));
		REQUIRE(state == GooseSuccess);
		REQUIRE(arrow_array.length == 1);
		REQUIRE(arrow_array.release);
		arrow_array.release(arrow_array_ptr);

		goose_destroy_arrow(&arrow_result);
		goose_destroy_prepare(&stmt);
	}

	SECTION("test scan") {
		const auto logical_types = goose::vector<LogicalType> {LogicalType(LogicalTypeId::INTEGER)};
		const auto column_names = goose::vector<string> {"value"};

		// arrow schema, release after use
		ArrowSchema arrow_schema;
		arrow_schema.Init();
		auto arrow_schema_ptr = &arrow_schema;

		ClientProperties options = (reinterpret_cast<Connection *>(tester.connection)->context->GetClientProperties());
		goose::ArrowConverter::ToArrowSchema(arrow_schema_ptr, logical_types, column_names, options);

		ArrowArray arrow_array;
		arrow_array.Init();
		auto arrow_array_ptr = &arrow_array;

		SECTION("empty array") {
			// Create an empty view with a `value` column.
			string view_name = "foo_empty_table";

			// arrow array scan, destroy out_stream after use
			ArrowArrayStream *arrow_array_stream;
			auto out_stream = reinterpret_cast<goose_arrow_stream *>(&arrow_array_stream);
			auto state = goose_arrow_array_scan(tester.connection, view_name.c_str(),
			                                     reinterpret_cast<goose_arrow_schema>(arrow_schema_ptr),
			                                     reinterpret_cast<goose_arrow_array>(arrow_array_ptr), out_stream);
			REQUIRE(state == GooseSuccess);

			// get the created view from the DB
			auto get_query = "SELECT * FROM " + view_name + ";";
			state = goose_prepare(tester.connection, get_query.c_str(), &stmt);
			REQUIRE(state == GooseSuccess);
			REQUIRE(stmt != nullptr);
			state = goose_execute_prepared_arrow(stmt, &arrow_result);
			REQUIRE(state == GooseSuccess);

			// recover the arrow array from the arrow result
			ArrowArray out_array;
			out_array.Init();
			auto out_array_ptr = &out_array;

			state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&out_array_ptr));
			REQUIRE(state == GooseSuccess);
			REQUIRE(out_array.length == 0);
			REQUIRE(out_array.release == nullptr);

			goose_destroy_arrow_stream(out_stream);
			REQUIRE(arrow_array.release == nullptr);
		}

		SECTION("big array") {
			// Create a view with a `value` column containing 4096 values.
			int num_buffers = 2, size = STANDARD_VECTOR_SIZE * num_buffers;
			unordered_map<idx_t, const goose::shared_ptr<ArrowTypeExtensionData>> extension_type_cast;
			ArrowAppender appender(logical_types, size, options, extension_type_cast);
			Allocator allocator;

			auto data_chunks = std::vector<DataChunk>(num_buffers);
			for (int i = 0; i < num_buffers; i++) {
				auto data_chunk = &data_chunks[i];
				data_chunk->Initialize(allocator, logical_types, STANDARD_VECTOR_SIZE);
				data_chunk->SetCardinality(STANDARD_VECTOR_SIZE);
				for (idx_t row = 0; row < STANDARD_VECTOR_SIZE; row++) {
					data_chunk->SetValue(0, row, goose::Value(i));
				}
				appender.Append(*data_chunk, 0, data_chunk->size(), data_chunk->size());
			}

			*arrow_array_ptr = appender.Finalize();

			// Create the view.
			string view_name = "foo_table";

			// arrow array scan, destroy out_stream after use
			ArrowArrayStream *arrow_array_stream;
			auto out_stream = reinterpret_cast<goose_arrow_stream *>(&arrow_array_stream);
			auto state = goose_arrow_array_scan(tester.connection, view_name.c_str(),
			                                     reinterpret_cast<goose_arrow_schema>(arrow_schema_ptr),
			                                     reinterpret_cast<goose_arrow_array>(arrow_array_ptr), out_stream);
			REQUIRE(state == GooseSuccess);

			// Get the created view from the DB.
			auto get_query = "SELECT * FROM " + view_name + ";";
			state = goose_prepare(tester.connection, get_query.c_str(), &stmt);
			REQUIRE(state == GooseSuccess);
			REQUIRE(stmt != nullptr);
			state = goose_execute_prepared_arrow(stmt, &arrow_result);
			REQUIRE(state == GooseSuccess);

			// Recover the arrow array from the arrow result.
			ArrowArray out_array;
			out_array.Init();
			auto out_array_ptr = &out_array;
			state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&out_array_ptr));
			REQUIRE(state == GooseSuccess);
			REQUIRE(out_array.length == STANDARD_VECTOR_SIZE);
			REQUIRE(out_array.release != nullptr);
			out_array.release(out_array_ptr);

			out_array.Init();
			state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&out_array_ptr));
			REQUIRE(state == GooseSuccess);
			REQUIRE(out_array.length == STANDARD_VECTOR_SIZE);
			REQUIRE(out_array.release != nullptr);
			out_array.release(out_array_ptr);

			out_array.Init();
			state = goose_query_arrow_array(arrow_result, reinterpret_cast<goose_arrow_array *>(&out_array_ptr));
			REQUIRE(state == GooseSuccess);
			REQUIRE(out_array.length == 0);
			REQUIRE(out_array.release == nullptr);

			goose_destroy_arrow_stream(out_stream);
			REQUIRE(arrow_array.release != nullptr);
		}

		SECTION("null schema") {
			// Creating a view with a null schema should fail gracefully.
			string view_name = "foo_empty_table_null_schema";

			// arrow array scan, destroy out_stream after use
			ArrowArrayStream *arrow_array_stream;
			auto out_stream = reinterpret_cast<goose_arrow_stream *>(&arrow_array_stream);
			auto state = goose_arrow_array_scan(tester.connection, view_name.c_str(), nullptr,
			                                     reinterpret_cast<goose_arrow_array>(arrow_array_ptr), out_stream);
			REQUIRE(state == GooseError);
			goose_destroy_arrow_stream(out_stream);
		}

		if (arrow_schema.release) {
			arrow_schema.release(arrow_schema_ptr);
		}
		if (arrow_array.release) {
			arrow_array.release(arrow_array_ptr);
		}

		goose_destroy_arrow(&arrow_result);
		goose_destroy_prepare(&stmt);
	}

	// FIXME: needs test for scanning a fixed size list
	// this likely requires nanoarrow to create the array to scan
}

TEST_CASE("Test C-API Arrow conversion functions", "[capi][arrow]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	SECTION("roundtrip: goose table -> arrow -> goose chunk, validate correctness") {
		// 1. Create and populate table
		REQUIRE_NO_FAIL(tester.Query("CREATE TABLE big_table(i INTEGER);"));
		REQUIRE_NO_FAIL(tester.Query("INSERT INTO big_table SELECT i FROM range(10000) tbl(i);"));

		// 2. Query the table and fetch all results as data chunks
		goose_result result;
		REQUIRE(goose_query(tester.connection, "SELECT i FROM big_table ORDER BY i", &result) == GooseSuccess);
		idx_t chunk_count = goose_result_chunk_count(result);
		idx_t total_rows = 0;
		std::vector<ArrowArray> arrow_arrays;
		std::vector<goose_data_chunk> goose_chunks;
		std::vector<int32_t> all_goose_values;
		std::vector<int32_t> all_arrow_values;

		// 3. For each chunk, convert to Arrow Array and collect values
		for (idx_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
			goose_data_chunk chunk = goose_result_get_chunk(result, chunk_idx);
			goose_chunks.push_back(chunk); // for later roundtrip
			idx_t chunk_size = goose_data_chunk_get_size(chunk);
			total_rows += chunk_size;
			auto vec = goose_data_chunk_get_vector(chunk, 0);
			auto data = static_cast<int32_t *>(goose_vector_get_data(vec));
			for (idx_t i = 0; i < chunk_size; i++) {
				all_goose_values.push_back(data[i]);
			}

			ArrowArray goose_arrow_array;
			goose_arrow_options arrow_options;
			goose_connection_get_arrow_options(tester.connection, &arrow_options);
			goose_error_data err = goose_data_chunk_to_arrow(arrow_options, chunk, &goose_arrow_array);
			goose_destroy_arrow_options(&arrow_options);
			REQUIRE(err == nullptr);
			arrow_arrays.push_back(goose_arrow_array);
		}
		REQUIRE(total_rows == 10000);
		REQUIRE(all_goose_values.size() == 10000);

		// 4. Prepare Arrow schema for roundtrip
		goose_logical_type type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
		goose_logical_type types[1] = {type};
		const char *names[1] = {strdup("i")};
		ArrowSchemaWrapper arrow_schema_wrapper;
		goose_arrow_options arrow_options;
		goose_connection_get_arrow_options(tester.connection, &arrow_options);
		goose_error_data err =
		    goose_to_arrow_schema(arrow_options, types, names, 1, &arrow_schema_wrapper.arrow_schema);
		goose_destroy_arrow_options(&arrow_options);
		REQUIRE(err == nullptr);
		goose_arrow_converted_schema converted_schema = nullptr;
		// Convert schema (simulate real use)

		err = goose_schema_from_arrow(tester.connection, &arrow_schema_wrapper.arrow_schema, &converted_schema);
		REQUIRE(err == nullptr);
		// 5. For each Arrow array, convert back to Goose chunk and validate
		for (size_t idx = 0, offset = 0; idx < arrow_arrays.size(); idx++) {
			ArrowArray *goose_arrow_array = &arrow_arrays[idx];
			// Prepare output chunk
			goose_data_chunk out_chunk;
			// Convert Arrow array to Goose chunk
			err = goose_data_chunk_from_arrow(tester.connection, goose_arrow_array, converted_schema, &out_chunk);
			REQUIRE(err == nullptr);
			idx_t chunk_size = goose_data_chunk_get_size(out_chunk);
			auto vec = goose_data_chunk_get_vector(out_chunk, 0);
			auto data = static_cast<int32_t *>(goose_vector_get_data(vec));
			for (idx_t i = 0; i < chunk_size; i++, offset++) {
				REQUIRE(data[i] == all_goose_values[offset]);
				all_arrow_values.push_back(data[i]);
			}
			goose_destroy_data_chunk(&out_chunk);
		}
		REQUIRE(all_arrow_values.size() == 10000);
		REQUIRE(all_arrow_values == all_goose_values);

		// 6. Cleanup
		free((void *)names[0]);
		goose_destroy_arrow_converted_schema(&converted_schema);
		for (auto arrow_array : arrow_arrays) {
			if (arrow_array.release) {
				arrow_array.release(&arrow_array);
			}
		}
		for (auto chunk : goose_chunks) {
			goose_destroy_data_chunk(&chunk);
		}
		goose_destroy_logical_type(&type);
		goose_destroy_result(&result);
	}

	SECTION("C-API Arrow Tess Null pointer inputs") {
		goose_error_data err;
		goose_logical_type type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
		const char *names[1] = {strdup("i")};
		// Test goose_to_arrow_schema
		ArrowSchema goose_arrow_schema;
		err = goose_to_arrow_schema(nullptr, &type, names, 1, &goose_arrow_schema);
		goose_arrow_options arrow_options;
		goose_connection_get_arrow_options(tester.connection, &arrow_options);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		err = goose_to_arrow_schema(arrow_options, nullptr, names, 1, &goose_arrow_schema);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		err = goose_to_arrow_schema(arrow_options, &type, nullptr, 1, &goose_arrow_schema);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		// Zero columns
		err = goose_to_arrow_schema(arrow_options, &type, names, 0, &goose_arrow_schema);
		REQUIRE(err == nullptr); // zero columns is allowed, but produces an empty schema
		if (goose_arrow_schema.release) {
			goose_arrow_schema.release(&goose_arrow_schema);
		}
		goose_destroy_logical_type(&type);

		// Test goose_data_chunk_to_arrow
		ArrowArray goose_arrow_array;

		goose_destroy_error_data(&err);
		err = goose_data_chunk_to_arrow(arrow_options, nullptr, &goose_arrow_array);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		err = goose_data_chunk_to_arrow(nullptr, nullptr, &goose_arrow_array);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);

		// Test goose_schema_from_arrow
		ArrowSchema schema;
		goose_arrow_converted_schema converted_schema = nullptr;
		err = goose_schema_from_arrow(nullptr, &schema, &converted_schema);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		err = goose_schema_from_arrow(tester.connection, &schema, nullptr);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);

		// Test goose_data_chunk_from_arrow
		ArrowArray arr;
		goose_data_chunk out_chunk = nullptr;
		err = goose_data_chunk_from_arrow(nullptr, &arr, converted_schema, &out_chunk);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		err = goose_data_chunk_from_arrow(tester.connection, &arr, nullptr, &out_chunk);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		err = goose_data_chunk_from_arrow(tester.connection, &arr, converted_schema, nullptr);
		REQUIRE(err != nullptr);
		goose_destroy_error_data(&err);
		goose_destroy_arrow_options(&arrow_options);
		free((void *)names[0]);
	}
}
