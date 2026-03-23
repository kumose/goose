#include "capi_tester.h"

using namespace goose;
using namespace std;

TEST_CASE("Test table_info incorrect 'is_valid' value for 'dflt_value' column", "[capi]") {
	goose_database db;
	goose_connection con;
	goose_result result;

	REQUIRE(goose_open(NULL, &db) != GooseError);
	REQUIRE(goose_connect(db, &con) != GooseError);
	//! Create a table with 40 columns
	REQUIRE(goose_query(con,
	                     "CREATE TABLE foo (c00 varchar, c01 varchar, c02 varchar, c03 varchar, c04 varchar, c05 "
	                     "varchar, c06 varchar, c07 varchar, c08 varchar, c09 varchar, c10 varchar, c11 varchar, c12 "
	                     "varchar, c13 varchar, c14 varchar, c15 varchar, c16 varchar, c17 varchar, c18 varchar, c19 "
	                     "varchar, c20 varchar, c21 varchar, c22 varchar, c23 varchar, c24 varchar, c25 varchar, c26 "
	                     "varchar, c27 varchar, c28 varchar, c29 varchar, c30 varchar, c31 varchar, c32 varchar, c33 "
	                     "varchar, c34 varchar, c35 varchar, c36 varchar, c37 varchar, c38 varchar, c39 varchar);",
	                     NULL) != GooseError);
	//! Get table info for the created table
	REQUIRE(goose_query(con, "PRAGMA table_info(foo);", &result) != GooseError);

	//! Columns ({cid, name, type, notnull, dflt_value, pk}}
	idx_t col_count = goose_column_count(&result);
	REQUIRE(col_count == 6);
	idx_t chunk_count = goose_result_chunk_count(result);

	// Loop over the produced chunks
	for (idx_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
		goose_data_chunk chunk = goose_result_get_chunk(result, chunk_idx);
		idx_t row_count = goose_data_chunk_get_size(chunk);

		for (idx_t row_idx = 0; row_idx < row_count; row_idx++) {
			for (idx_t col_idx = 0; col_idx < col_count; col_idx++) {
				//! Get the column
				goose_vector vector = goose_data_chunk_get_vector(chunk, col_idx);
				uint64_t *validity = goose_vector_get_validity(vector);
				bool is_valid = goose_validity_row_is_valid(validity, row_idx);

				if (col_idx == 4) {
					//'dflt_value' column
					REQUIRE(is_valid == false);
				}
			}
		}
		goose_destroy_data_chunk(&chunk);
	}

	goose_destroy_result(&result);
	goose_disconnect(&con);
	goose_close(&db);
}

TEST_CASE("Test Logical Types C API", "[capi]") {
	goose_logical_type type = goose_create_logical_type(GOOSE_TYPE_BIGINT);
	REQUIRE(type);
	REQUIRE(goose_get_type_id(type) == GOOSE_TYPE_BIGINT);
	goose_destroy_logical_type(&type);
	goose_destroy_logical_type(&type);

	// list type
	goose_logical_type elem_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	goose_logical_type list_type = goose_create_list_type(elem_type);
	REQUIRE(goose_get_type_id(list_type) == GOOSE_TYPE_LIST);
	goose_logical_type elem_type_dup = goose_list_type_child_type(list_type);
	REQUIRE(elem_type_dup != elem_type);
	REQUIRE(goose_get_type_id(elem_type_dup) == goose_get_type_id(elem_type));
	goose_destroy_logical_type(&elem_type);
	goose_destroy_logical_type(&list_type);
	goose_destroy_logical_type(&elem_type_dup);

	// map type
	goose_logical_type key_type = goose_create_logical_type(GOOSE_TYPE_SMALLINT);
	goose_logical_type value_type = goose_create_logical_type(GOOSE_TYPE_DOUBLE);
	goose_logical_type map_type = goose_create_map_type(key_type, value_type);
	REQUIRE(goose_get_type_id(map_type) == GOOSE_TYPE_MAP);
	goose_logical_type key_type_dup = goose_map_type_key_type(map_type);
	goose_logical_type value_type_dup = goose_map_type_value_type(map_type);
	REQUIRE(key_type_dup != key_type);
	REQUIRE(value_type_dup != value_type);
	REQUIRE(goose_get_type_id(key_type_dup) == goose_get_type_id(key_type));
	REQUIRE(goose_get_type_id(value_type_dup) == goose_get_type_id(value_type));
	goose_destroy_logical_type(&key_type);
	goose_destroy_logical_type(&value_type);
	goose_destroy_logical_type(&map_type);
	goose_destroy_logical_type(&key_type_dup);
	goose_destroy_logical_type(&value_type_dup);

	goose_destroy_logical_type(nullptr);
}

TEST_CASE("Test DataChunk C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	goose_state status;

	REQUIRE(tester.OpenDatabase(nullptr));
	REQUIRE(goose_vector_size() == STANDARD_VECTOR_SIZE);

	// create column types
	const idx_t COLUMN_COUNT = 3;
	goose_type gooseTypes[COLUMN_COUNT];
	gooseTypes[0] = GOOSE_TYPE_BIGINT;
	gooseTypes[1] = GOOSE_TYPE_SMALLINT;
	gooseTypes[2] = GOOSE_TYPE_BLOB;

	goose_logical_type types[COLUMN_COUNT];
	for (idx_t i = 0; i < COLUMN_COUNT; i++) {
		types[i] = goose_create_logical_type(gooseTypes[i]);
	}

	// create data chunk
	auto data_chunk = goose_create_data_chunk(types, COLUMN_COUNT);
	REQUIRE(data_chunk);
	REQUIRE(goose_data_chunk_get_column_count(data_chunk) == COLUMN_COUNT);

	// test goose_vector_get_column_type
	for (idx_t i = 0; i < COLUMN_COUNT; i++) {
		auto vector = goose_data_chunk_get_vector(data_chunk, i);
		auto type = goose_vector_get_column_type(vector);
		REQUIRE(goose_get_type_id(type) == gooseTypes[i]);
		goose_destroy_logical_type(&type);
	}

	REQUIRE(goose_data_chunk_get_vector(data_chunk, 999) == nullptr);
	REQUIRE(goose_data_chunk_get_vector(nullptr, 0) == nullptr);
	REQUIRE(goose_vector_get_column_type(nullptr) == nullptr);
	REQUIRE(goose_data_chunk_get_size(data_chunk) == 0);
	REQUIRE(goose_data_chunk_get_size(nullptr) == 0);

	// create table
	tester.Query("CREATE TABLE test(i BIGINT, j SMALLINT, k BLOB)");

	// use the appender to insert values using the data chunk API
	goose_appender appender;
	status = goose_appender_create(tester.connection, nullptr, "test", &appender);
	REQUIRE(status == GooseSuccess);

	// get the column types from the appender
	REQUIRE(goose_appender_column_count(appender) == COLUMN_COUNT);

	// test goose_appender_column_type
	for (idx_t i = 0; i < COLUMN_COUNT; i++) {
		auto type = goose_appender_column_type(appender, i);
		REQUIRE(goose_get_type_id(type) == gooseTypes[i]);
		goose_destroy_logical_type(&type);
	}

	// append BIGINT
	auto bigint_vector = goose_data_chunk_get_vector(data_chunk, 0);
	auto int64_ptr = (int64_t *)goose_vector_get_data(bigint_vector);
	*int64_ptr = 42;

	// append SMALLINT
	auto smallint_vector = goose_data_chunk_get_vector(data_chunk, 1);
	auto int16_ptr = (int16_t *)goose_vector_get_data(smallint_vector);
	*int16_ptr = 84;

	// append BLOB
	string s = "this is my blob";
	auto blob_vector = goose_data_chunk_get_vector(data_chunk, 2);
	goose_vector_assign_string_element_len(blob_vector, 0, s.c_str(), s.length());

	REQUIRE(goose_vector_get_data(nullptr) == nullptr);

	goose_data_chunk_set_size(data_chunk, 1);
	REQUIRE(goose_data_chunk_get_size(data_chunk) == 1);

	REQUIRE(goose_append_data_chunk(appender, data_chunk) == GooseSuccess);
	REQUIRE(goose_append_data_chunk(appender, nullptr) == GooseError);
	REQUIRE(goose_append_data_chunk(nullptr, data_chunk) == GooseError);

	// append nulls
	goose_data_chunk_reset(data_chunk);
	REQUIRE(goose_data_chunk_get_size(data_chunk) == 0);

	for (idx_t i = 0; i < COLUMN_COUNT; i++) {
		auto vector = goose_data_chunk_get_vector(data_chunk, i);
		goose_vector_ensure_validity_writable(vector);
		auto validity = goose_vector_get_validity(vector);

		REQUIRE(goose_validity_row_is_valid(validity, 0));
		goose_validity_set_row_validity(validity, 0, false);
		REQUIRE(!goose_validity_row_is_valid(validity, 0));
	}

	goose_data_chunk_set_size(data_chunk, 1);
	REQUIRE(goose_data_chunk_get_size(data_chunk) == 1);
	REQUIRE(goose_append_data_chunk(appender, data_chunk) == GooseSuccess);
	REQUIRE(goose_vector_get_validity(nullptr) == nullptr);

	goose_appender_destroy(&appender);

	result = tester.Query("SELECT * FROM test");
	REQUIRE_NO_FAIL(*result);

	REQUIRE(result->Fetch<int64_t>(0, 0) == 42);
	REQUIRE(result->Fetch<int16_t>(1, 0) == 84);
	REQUIRE(result->Fetch<string>(2, 0) == "this is my blob");

	REQUIRE(result->IsNull(0, 1));
	REQUIRE(result->IsNull(1, 1));
	REQUIRE(result->IsNull(2, 1));

	goose_data_chunk_reset(data_chunk);
	goose_data_chunk_reset(nullptr);
	REQUIRE(goose_data_chunk_get_size(data_chunk) == 0);

	goose_destroy_data_chunk(&data_chunk);
	goose_destroy_data_chunk(&data_chunk);
	goose_destroy_data_chunk(nullptr);

	for (idx_t i = 0; i < COLUMN_COUNT; i++) {
		goose_destroy_logical_type(&types[i]);
	}
}

TEST_CASE("Test DataChunk varchar result fetch in C API", "[capi]") {
	if (goose_vector_size() < 64) {
		return;
	}

	goose_database database;
	goose_connection connection;
	goose_state state;

	state = goose_open(nullptr, &database);
	REQUIRE(state == GooseSuccess);
	state = goose_connect(database, &connection);
	REQUIRE(state == GooseSuccess);

	constexpr const char *VARCHAR_TEST_QUERY = "select case when i != 0 and i % 42 = 0 then NULL else repeat(chr((65 + "
	                                           "(i % 26))::INTEGER), (4 + (i % 12))) end from range(5000) tbl(i);";

	// fetch a small result set
	goose_result result;
	state = goose_query(connection, VARCHAR_TEST_QUERY, &result);
	REQUIRE(state == GooseSuccess);

	REQUIRE(goose_column_count(&result) == 1);
	REQUIRE(goose_row_count(&result) == 5000);
	REQUIRE(goose_result_error(&result) == nullptr);

	idx_t expected_chunk_count = (5000 / STANDARD_VECTOR_SIZE) + (5000 % STANDARD_VECTOR_SIZE != 0);

	REQUIRE(goose_result_chunk_count(result) == expected_chunk_count);

	auto chunk = goose_result_get_chunk(result, 0);

	REQUIRE(goose_data_chunk_get_column_count(chunk) == 1);
	REQUIRE(STANDARD_VECTOR_SIZE < 5000);
	REQUIRE(goose_data_chunk_get_size(chunk) == STANDARD_VECTOR_SIZE);
	goose_destroy_data_chunk(&chunk);

	idx_t tuple_index = 0;
	auto chunk_amount = goose_result_chunk_count(result);
	for (idx_t chunk_index = 0; chunk_index < chunk_amount; chunk_index++) {
		chunk = goose_result_get_chunk(result, chunk_index);
		// Our result only has one column
		auto vector = goose_data_chunk_get_vector(chunk, 0);
		auto validity = goose_vector_get_validity(vector);
		auto string_data = (goose_string_t *)goose_vector_get_data(vector);

		auto tuples_in_chunk = goose_data_chunk_get_size(chunk);
		for (idx_t i = 0; i < tuples_in_chunk; i++, tuple_index++) {
			if (!goose_validity_row_is_valid(validity, i)) {
				// This entry is NULL
				REQUIRE((tuple_index != 0 && tuple_index % 42 == 0));
				continue;
			}
			idx_t expected_length = (tuple_index % 12) + 4;
			char expected_character = (tuple_index % 26) + 'A';

			// TODO: how does the c-api handle non-flat vectors?
			auto tuple = string_data[i];
			auto length = tuple.value.inlined.length;
			REQUIRE(length == expected_length);
			if (goose_string_is_inlined(tuple)) {
				// The data is small enough to fit in the string_t, it does not have a separate allocation
				for (idx_t string_index = 0; string_index < length; string_index++) {
					REQUIRE(tuple.value.inlined.inlined[string_index] == expected_character);
				}
			} else {
				for (idx_t string_index = 0; string_index < length; string_index++) {
					REQUIRE(tuple.value.pointer.ptr[string_index] == expected_character);
				}
			}
		}
		goose_destroy_data_chunk(&chunk);
	}
	goose_destroy_result(&result);
	goose_disconnect(&connection);
	goose_close(&database);
}

TEST_CASE("Test DataChunk result fetch in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	if (goose_vector_size() < 64) {
		return;
	}

	REQUIRE(tester.OpenDatabase(nullptr));

	// fetch a small result set
	result = tester.Query("SELECT CASE WHEN i=1 THEN NULL ELSE i::INTEGER END i FROM range(3) tbl(i)");
	REQUIRE(NO_FAIL(*result));
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 3);
	REQUIRE(result->ErrorMessage() == nullptr);

	// fetch the first chunk
	REQUIRE(result->ChunkCount() == 1);
	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);

	REQUIRE(chunk->ColumnCount() == 1);
	REQUIRE(chunk->size() == 3);

	auto data = (int32_t *)chunk->GetData(0);
	auto validity = chunk->GetValidity(0);
	REQUIRE(data[0] == 0);
	REQUIRE(data[2] == 2);
	REQUIRE(goose_validity_row_is_valid(validity, 0));
	REQUIRE(!goose_validity_row_is_valid(validity, 1));
	REQUIRE(goose_validity_row_is_valid(validity, 2));

	// after fetching a chunk, we cannot use the old API anymore
	REQUIRE(result->ColumnData<int32_t>(0) == nullptr);
	REQUIRE(result->Fetch<int32_t>(0, 1) == 0);

	// result set is exhausted!
	chunk = result->FetchChunk(1);
	REQUIRE(!chunk);
}

TEST_CASE("Test goose_result_return_type", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));

	result = tester.Query("CREATE TABLE t (id INT)");
	REQUIRE(goose_result_return_type(result->InternalResult()) == GOOSE_RESULT_TYPE_NOTHING);

	result = tester.Query("INSERT INTO t VALUES (42)");
	REQUIRE(goose_result_return_type(result->InternalResult()) == GOOSE_RESULT_TYPE_CHANGED_ROWS);

	result = tester.Query("FROM t");
	REQUIRE(goose_result_return_type(result->InternalResult()) == GOOSE_RESULT_TYPE_QUERY_RESULT);
}

TEST_CASE("Test DataChunk populate ListVector in C API", "[capi]") {
	if (goose_vector_size() < 3) {
		return;
	}
	REQUIRE(goose_list_vector_reserve(nullptr, 100) == goose_state::GooseError);
	REQUIRE(goose_list_vector_set_size(nullptr, 200) == goose_state::GooseError);

	auto elem_type = goose_create_logical_type(goose_type::GOOSE_TYPE_INTEGER);
	auto list_type = goose_create_list_type(elem_type);
	goose_logical_type schema[] = {list_type};
	auto chunk = goose_create_data_chunk(schema, 1);
	auto list_vector = goose_data_chunk_get_vector(chunk, 0);
	goose_data_chunk_set_size(chunk, 3);

	REQUIRE(goose_list_vector_reserve(list_vector, 123) == goose_state::GooseSuccess);
	REQUIRE(goose_list_vector_get_size(list_vector) == 0);
	auto child = goose_list_vector_get_child(list_vector);
	for (int i = 0; i < 123; i++) {
		((int *)goose_vector_get_data(child))[i] = i;
	}
	REQUIRE(goose_list_vector_set_size(list_vector, 123) == goose_state::GooseSuccess);
	REQUIRE(goose_list_vector_get_size(list_vector) == 123);

	auto entries = (goose_list_entry *)goose_vector_get_data(list_vector);
	entries[0].offset = 0;
	entries[0].length = 20;
	entries[1].offset = 20;
	entries[1].length = 80;
	entries[2].offset = 100;
	entries[2].length = 23;

	auto child_data = (int *)goose_vector_get_data(child);
	int count = 0;
	for (idx_t i = 0; i < goose_data_chunk_get_size(chunk); i++) {
		for (idx_t j = 0; j < entries[i].length; j++) {
			REQUIRE(child_data[entries[i].offset + j] == count);
			count++;
		}
	}
	auto &vector = (Vector &)(*list_vector);
	for (int i = 0; i < 123; i++) {
		REQUIRE(ListVector::GetEntry(vector).GetValue(i) == i);
	}

	goose_destroy_data_chunk(&chunk);
	goose_destroy_logical_type(&list_type);
	goose_destroy_logical_type(&elem_type);
}

TEST_CASE("Test DataChunk populate ArrayVector in C API", "[capi]") {
	auto elem_type = goose_create_logical_type(goose_type::GOOSE_TYPE_INTEGER);
	auto array_type = goose_create_array_type(elem_type, 3);
	goose_logical_type schema[] = {array_type};
	auto chunk = goose_create_data_chunk(schema, 1);
	goose_data_chunk_set_size(chunk, 2);
	auto array_vector = goose_data_chunk_get_vector(chunk, 0);

	auto child = goose_array_vector_get_child(array_vector);
	for (int i = 0; i < 6; i++) {
		((int *)goose_vector_get_data(child))[i] = i;
	}

	auto vec = (Vector &)(*array_vector);
	for (int i = 0; i < 2; i++) {
		auto child_vals = ArrayValue::GetChildren(vec.GetValue(i));
		for (int j = 0; j < 3; j++) {
			REQUIRE(child_vals[j].GetValue<int>() == i * 3 + j);
		}
	}

	goose_destroy_data_chunk(&chunk);
	goose_destroy_logical_type(&array_type);
	goose_destroy_logical_type(&elem_type);
}

TEST_CASE("Test PK violation in the C API appender", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));
	REQUIRE(goose_vector_size() == STANDARD_VECTOR_SIZE);

	// Create column types.
	const idx_t COLUMN_COUNT = 1;
	goose_logical_type types[COLUMN_COUNT];
	types[0] = goose_create_logical_type(GOOSE_TYPE_BIGINT);

	// Create data chunk.
	auto data_chunk = goose_create_data_chunk(types, COLUMN_COUNT);
	auto bigint_vector = goose_data_chunk_get_vector(data_chunk, 0);
	auto int64_ptr = reinterpret_cast<int64_t *>(goose_vector_get_data(bigint_vector));
	int64_ptr[0] = 42;
	int64_ptr[1] = 42;
	goose_data_chunk_set_size(data_chunk, 2);

	// Use the appender to append the data chunk.
	tester.Query("CREATE TABLE test(i BIGINT PRIMARY KEY)");
	goose_appender appender;
	REQUIRE(goose_appender_create(tester.connection, nullptr, "test", &appender) == GooseSuccess);

	// We only flush when destroying the appender. Thus, we expect this to succeed, as we only
	// detect constraint violations when flushing the results.
	REQUIRE(goose_append_data_chunk(appender, data_chunk) == GooseSuccess);

	// goose_appender_close attempts to flush the data and fails.
	auto state = goose_appender_close(appender);
	REQUIRE(state == GooseError);
	auto error = goose_appender_error(appender);
	REQUIRE(goose::StringUtil::Contains(error, "PRIMARY KEY or UNIQUE constraint violation"));

	// Destroy the appender despite the error to avoid leaks.
	state = goose_appender_destroy(&appender);
	REQUIRE(state == GooseError);

	// Clean-up.
	goose_destroy_data_chunk(&data_chunk);
	for (idx_t i = 0; i < COLUMN_COUNT; i++) {
		goose_destroy_logical_type(&types[i]);
	}

	// Ensure that no rows were appended.
	result = tester.Query("SELECT * FROM test;");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->row_count() == 0);

	// Try again by appending rows and flushing.
	REQUIRE(goose_appender_create(tester.connection, nullptr, "test", &appender) == GooseSuccess);
	REQUIRE(goose_appender_begin_row(appender) == GooseSuccess);
	REQUIRE(goose_append_int64(appender, 42) == GooseSuccess);
	REQUIRE(goose_appender_end_row(appender) == GooseSuccess);
	REQUIRE(goose_appender_begin_row(appender) == GooseSuccess);
	REQUIRE(goose_append_int64(appender, 42) == GooseSuccess);
	REQUIRE(goose_appender_end_row(appender) == GooseSuccess);

	state = goose_appender_flush(appender);
	REQUIRE(state == GooseError);
	error = goose_appender_error(appender);
	REQUIRE(goose::StringUtil::Contains(error, "PRIMARY KEY or UNIQUE constraint violation"));
	REQUIRE(goose_appender_destroy(&appender) == GooseError);

	// Ensure that only the last row was appended.
	result = tester.Query("SELECT * FROM test;");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->row_count() == 0);
}

TEST_CASE("Test DataChunk write BLOB", "[capi]") {
	goose_logical_type type = goose_create_logical_type(GOOSE_TYPE_BLOB);
	REQUIRE(type);
	REQUIRE(goose_get_type_id(type) == GOOSE_TYPE_BLOB);
	goose_logical_type types[] = {type};
	auto chunk = goose_create_data_chunk(types, 1);
	goose_data_chunk_set_size(chunk, 1);
	goose_vector vector = goose_data_chunk_get_vector(chunk, 0);
	auto column_type = goose_vector_get_column_type(vector);
	REQUIRE(goose_get_type_id(column_type) == GOOSE_TYPE_BLOB);
	goose_destroy_logical_type(&column_type);
	uint8_t bytes[] = {0x80, 0x00, 0x01, 0x2a};
	goose_vector_assign_string_element_len(vector, 0, (const char *)bytes, 4);
	auto string_data = static_cast<goose_string_t *>(goose_vector_get_data(vector));
	auto string_value = goose_string_t_data(string_data);
	REQUIRE(goose_string_t_length(*string_data) == 4);
	REQUIRE(string_value[0] == (char)0x80);
	REQUIRE(string_value[1] == (char)0x00);
	REQUIRE(string_value[2] == (char)0x01);
	REQUIRE(string_value[3] == (char)0x2a);
	goose_destroy_data_chunk(&chunk);
	goose_destroy_logical_type(&type);
}

TEST_CASE("Test DataChunk write BIGNUM", "[capi]") {
	goose_logical_type type = goose_create_logical_type(GOOSE_TYPE_BIGNUM);
	REQUIRE(type);
	REQUIRE(goose_get_type_id(type) == GOOSE_TYPE_BIGNUM);
	goose_logical_type types[] = {type};
	auto chunk = goose_create_data_chunk(types, 1);
	goose_data_chunk_set_size(chunk, 1);
	goose_vector vector = goose_data_chunk_get_vector(chunk, 0);
	auto column_type = goose_vector_get_column_type(vector);
	REQUIRE(goose_get_type_id(column_type) == GOOSE_TYPE_BIGNUM);
	goose_destroy_logical_type(&column_type);
	uint8_t bytes[] = {0x80, 0x00, 0x01, 0x2a}; // BIGNUM 42
	goose_vector_assign_string_element_len(vector, 0, (const char *)bytes, 4);
	auto string_data = static_cast<goose_string_t *>(goose_vector_get_data(vector));
	auto string_value = goose_string_t_data(string_data);
	REQUIRE(goose_string_t_length(*string_data) == 4);
	REQUIRE(string_value[0] == (char)0x80);
	REQUIRE(string_value[1] == (char)0x00);
	REQUIRE(string_value[2] == (char)0x01);
	REQUIRE(string_value[3] == (char)0x2a);
	goose_destroy_data_chunk(&chunk);
	goose_destroy_logical_type(&type);
}
