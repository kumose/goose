// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "catch.h"
#include <goose/goose-c.h>
#include "test_helpers.h"
#include <goose/common/arrow/arrow.h>
#include <goose/common/exception.h>

namespace goose {

class CAPIDataChunk {
public:
	CAPIDataChunk(goose_data_chunk chunk_p) : chunk(chunk_p) {
	}
	~CAPIDataChunk() {
		goose_destroy_data_chunk(&chunk);
	}

	idx_t ColumnCount() {
		return goose_data_chunk_get_column_count(chunk);
	}
	idx_t size() {
		return goose_data_chunk_get_size(chunk);
	}
	goose_vector GetVector(idx_t col) {
		return goose_data_chunk_get_vector(chunk, col);
	}
	void *GetData(idx_t col) {
		return goose_vector_get_data(GetVector(col));
	}
	uint64_t *GetValidity(idx_t col) {
		return goose_vector_get_validity(GetVector(col));
	}
	goose_vector GetListChildVector(idx_t col) {
		return goose_list_vector_get_child(GetVector(col));
	}
	void *GetListChildData(idx_t col) {
		return goose_vector_get_data(GetListChildVector(col));
	}
	goose_vector GetStructChildVector(idx_t col, idx_t idx) {
		return goose_struct_vector_get_child(GetVector(col), idx);
	}
	void *GetStructChildData(idx_t col, idx_t idx) {
		return goose_vector_get_data(GetStructChildVector(col, idx));
	}
	goose_vector GetArrayChildVector(idx_t col) {
		return goose_array_vector_get_child(GetVector(col));
	}
	void *GetArrayChildData(idx_t col) {
		return goose_vector_get_data(GetArrayChildVector(col));
	}
	goose_data_chunk GetChunk() {
		return chunk;
	}

private:
	goose_data_chunk chunk;
};

class CAPIResult {
public:
	CAPIResult() {
	}
	CAPIResult(goose_result result, bool success) : success(success), result(result) {
	}
	~CAPIResult() {
		goose_destroy_result(&result);
	}

public:
	bool HasError() const {
		return !success;
	}
	void Query(goose_connection connection, string query) {
		success = (goose_query(connection, query.c_str(), &result) == GooseSuccess);
		if (!success) {
			REQUIRE(ErrorMessage() != nullptr);
			REQUIRE(ErrorType() != GOOSE_ERROR_INVALID);
		}
	}
	void QueryPrepared(goose_prepared_statement statement) {
		success = goose_execute_prepared(statement, &result) == GooseSuccess;
		if (!success) {
			REQUIRE(ErrorMessage() != nullptr);
			REQUIRE(ErrorType() != GOOSE_ERROR_INVALID);
		}
	}

	goose_type ColumnType(idx_t col) {
		return goose_column_type(&result, col);
	}

	idx_t ChunkCount() {
		return goose_result_chunk_count(result);
	}

	unique_ptr<CAPIDataChunk> StreamChunk() {
		auto chunk = goose_stream_fetch_chunk(result);
		if (!chunk) {
			return nullptr;
		}
		return make_uniq<CAPIDataChunk>(chunk);
	}

	bool IsStreaming() {
		return goose_result_is_streaming(result);
	}

	unique_ptr<CAPIDataChunk> FetchChunk(idx_t chunk_idx) {
		auto chunk = goose_result_get_chunk(result, chunk_idx);
		if (!chunk) {
			return nullptr;
		}
		return make_uniq<CAPIDataChunk>(chunk);
	}

	unique_ptr<CAPIDataChunk> NextChunk() {
		auto chunk = goose_fetch_chunk(result);
		if (!chunk) {
			return nullptr;
		}
		return make_uniq<CAPIDataChunk>(chunk);
	}

	template <class T>
	T *ColumnData(idx_t col) {
		return (T *)goose_column_data(&result, col);
	}

	idx_t ColumnCount() {
		return goose_column_count(&result);
	}

	idx_t row_count() {
		return goose_row_count(&result);
	}

	idx_t rows_changed() {
		return goose_rows_changed(&result);
	}

	template <class T>
	T Fetch(idx_t col, idx_t row) {
		throw NotImplementedException("Unimplemented type for fetch");
	}

	bool IsNull(idx_t col, idx_t row) {
		auto nullmask_ptr = goose_nullmask_data(&result, col);
		REQUIRE(goose_value_is_null(&result, col, row) == nullmask_ptr[row]);
		return nullmask_ptr[row];
	}

	const char *ErrorMessage() {
		return goose_result_error(&result);
	}
	goose_error_type ErrorType() {
		return goose_result_error_type(&result);
	}

	string ColumnName(idx_t col) {
		auto colname = goose_column_name(&result, col);
		return colname ? string(colname) : string();
	}

	goose_result &InternalResult() {
		return result;
	}

public:
	bool success = false;

protected:
	goose_result result;
};

template <>
bool CAPIResult::Fetch(idx_t col, idx_t row);
template <>
int8_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
int16_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
int32_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
int64_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
uint8_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
uint16_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
uint32_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
uint64_t CAPIResult::Fetch(idx_t col, idx_t row);
template <>
float CAPIResult::Fetch(idx_t col, idx_t row);
template <>
double CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_decimal CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_date CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_time CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_time_ns CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_timestamp CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_timestamp_s CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_timestamp_ms CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_timestamp_ns CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_interval CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_blob CAPIResult::Fetch(idx_t col, idx_t row);
template <>
string CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_date_struct CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_time_struct CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_timestamp_struct CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_hugeint CAPIResult::Fetch(idx_t col, idx_t row);
template <>
goose_uhugeint CAPIResult::Fetch(idx_t col, idx_t row);

class CAPITester {
public:
	CAPITester() : database(nullptr), connection(nullptr) {
	}
	~CAPITester() {
		Cleanup();
	}

	void Cleanup() {
		if (connection) {
			goose_disconnect(&connection);
			connection = nullptr;
		}
		if (database) {
			goose_close(&database);
			database = nullptr;
		}
	}

	bool OpenDatabase(const char *path) {
		Cleanup();
		if (goose_open(path, &database) != GooseSuccess) {
			return false;
		}
		return goose_connect(database, &connection) == GooseSuccess;
	}

	bool ChangeConnection() {
		goose_disconnect(&connection);
		return goose_connect(database, &connection) == GooseSuccess;
	}

	goose::unique_ptr<CAPIResult> Query(string query) {
		D_ASSERT(connection);
		auto result = make_uniq<CAPIResult>();
		result->Query(connection, query);
		return result;
	}
	goose::unique_ptr<CAPIResult> QueryPrepared(goose_prepared_statement prepared) {
		D_ASSERT(connection);
		auto result = make_uniq<CAPIResult>();
		result->QueryPrepared(prepared);
		return result;
	}

	goose_database database = nullptr;
	goose_connection connection = nullptr;
};

struct CAPIPrepared {
	CAPIPrepared() {
	}
	~CAPIPrepared() {
		if (!prepared) {
			return;
		}
		goose_destroy_prepare(&prepared);
	}

	bool Prepare(CAPITester &tester, const string &query) {
		auto state = goose_prepare(tester.connection, query.c_str(), &prepared);
		return state == GooseSuccess;
	}

	goose_prepared_statement prepared = nullptr;
};

struct CAPIPending {
	CAPIPending() {
	}
	~CAPIPending() {
		if (!pending) {
			return;
		}
		goose_destroy_pending(&pending);
	}

	bool Pending(CAPIPrepared &prepared) {
		auto state = goose_pending_prepared(prepared.prepared, &pending);
		return state == GooseSuccess;
	}

	bool PendingStreaming(CAPIPrepared &prepared) {
		auto state = goose_pending_prepared_streaming(prepared.prepared, &pending);
		return state == GooseSuccess;
	}

	goose_pending_state ExecuteTask() {
		REQUIRE(pending);
		return goose_pending_execute_task(pending);
	}

	unique_ptr<CAPIResult> Execute() {
		goose_result result;
		auto success = goose_execute_pending(pending, &result) == GooseSuccess;
		return make_uniq<CAPIResult>(result, success);
	}

	goose_pending_result pending = nullptr;
};

} // namespace goose

bool NO_FAIL(goose::CAPIResult &result);
bool NO_FAIL(goose::unique_ptr<goose::CAPIResult> result);
