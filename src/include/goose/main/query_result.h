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

#include <goose/common/enums/statement_type.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/winapi.h>
#include <goose/common/error_data.h>
#include <goose/main/client_properties.h>

namespace goose {
struct BoxRendererConfig;

enum class QueryResultType : uint8_t { MATERIALIZED_RESULT, STREAM_RESULT, PENDING_RESULT, ARROW_RESULT };

class BaseQueryResult {
public:
	//! Creates a successful query result with the specified names and types
	GOOSE_API BaseQueryResult(QueryResultType type, StatementType statement_type, StatementProperties properties,
	                           vector<LogicalType> types, vector<string> names);
	//! Creates an unsuccessful query result with error condition
	GOOSE_API BaseQueryResult(QueryResultType type, ErrorData error);
	GOOSE_API virtual ~BaseQueryResult();

	//! The type of the result (MATERIALIZED or STREAMING)
	QueryResultType type;
	//! The type of the statement that created this result
	StatementType statement_type;
	//! Properties of the statement
	StatementProperties properties;
	//! The SQL types of the result
	vector<LogicalType> types;
	//! The names of the result
	vector<string> names;

public:
	[[noreturn]] GOOSE_API void ThrowError(const string &prepended_message = "") const;
	GOOSE_API void SetError(ErrorData error);
	GOOSE_API bool HasError() const;
	GOOSE_API const ExceptionType &GetErrorType() const;
	GOOSE_API const std::string &GetError() const;
	GOOSE_API ErrorData &GetErrorObject();
	GOOSE_API idx_t ColumnCount();

protected:
	//! Whether or not execution was successful
	bool success;
	//! The error (in case execution was not successful)
	ErrorData error;
};

//! The QueryResult object holds the result of a query. It can either be a MaterializedQueryResult, in which case the
//! result contains the entire result set, or a StreamQueryResult in which case the Fetch method can be called to
//! incrementally fetch data from the database.
class QueryResult : public BaseQueryResult {
public:
	//! Creates a successful query result with the specified names and types
	GOOSE_API QueryResult(QueryResultType type, StatementType statement_type, StatementProperties properties,
	                       vector<LogicalType> types, vector<string> names, ClientProperties client_properties);
	//! Creates an unsuccessful query result with error condition
	GOOSE_API QueryResult(QueryResultType type, ErrorData error);
	GOOSE_API ~QueryResult() override;

	//! Properties from the client context
	ClientProperties client_properties;
	//! The next result (if any)
	unique_ptr<QueryResult> next;

public:
	template <class TARGET>
	TARGET &Cast() {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast query result to type - query result type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast query result to type - query result type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}

public:
	//! Deduplicate column names for interop with external libraries
	static void DeduplicateColumns(vector<string> &names);

public:
	//! Returns the name of the column for the given index
	GOOSE_API const string &ColumnName(idx_t index) const;
	//! Fetches a DataChunk of normalized (flat) vectors from the query result.
	//! Returns nullptr if there are no more results to fetch.
	GOOSE_API unique_ptr<DataChunk> Fetch();
	//! Fetches a DataChunk from the query result. The vectors are not normalized and hence any vector types can be
	//! returned.
	GOOSE_API unique_ptr<DataChunk> FetchRaw();
	//! Converts the QueryResult to a string
	GOOSE_API virtual string ToString() = 0;
	//! Converts the QueryResult to a box-rendered string
	GOOSE_API virtual string ToBox(ClientContext &context, const BoxRendererConfig &config);
	//! Prints the QueryResult to the console
	GOOSE_API void Print();
	//! Returns true if the two results are identical; false otherwise. Note that this method is destructive; it calls
	//! Fetch() until both results are exhausted. The data in the results will be lost.
	GOOSE_API bool Equals(QueryResult &other);

	bool TryFetch(unique_ptr<DataChunk> &result, ErrorData &error) {
		try {
			result = Fetch();
			return success;
		} catch (std::exception &ex) {
			error = ErrorData(ex);
			return false;
		} catch (...) {
			error = ErrorData("Unknown error in Fetch");
			return false;
		}
	}

protected:
	GOOSE_API virtual unique_ptr<DataChunk> FetchInternal() = 0;

private:
	class QueryResultIterator;
	class QueryResultRow {
		friend class QueryResultIterator;

	public:
		explicit QueryResultRow() : row(0) {
		}

		bool IsNull(idx_t col_idx) const {
			return chunk->GetValue(col_idx, row).IsNull();
		}
		template <class T>
		T GetValue(idx_t col_idx) const {
			return chunk->GetValue(col_idx, row).GetValue<T>();
		}
		Value GetBaseValue(idx_t col_idx) const {
			return chunk->GetValue(col_idx, row);
		}
		DataChunk &GetChunk() const {
			return *chunk;
		}
		idx_t GetRowInChunk() const {
			return row;
		}

	private:
		shared_ptr<DataChunk> chunk;
		idx_t row;
	};
	//! The row-based query result iterator. Invoking the
	class QueryResultIterator {
	public:
		explicit QueryResultIterator(optional_ptr<QueryResult> result_p = nullptr) : result(result_p), base_row(0) {
			if (result) {
				current_row.chunk = shared_ptr<DataChunk>(result->Fetch().release());
				if (!current_row.chunk) {
					result = nullptr;
				}
			}
		}

		QueryResultRow current_row;
		optional_ptr<QueryResult> result;
		idx_t base_row;

	public:
		void Next() {
			if (!current_row.chunk) {
				return;
			}
			current_row.row++;
			if (current_row.row >= current_row.chunk->size()) {
				base_row += current_row.chunk->size();
				current_row.chunk = shared_ptr<DataChunk>(result->Fetch().release());
				current_row.row = 0;
				if (!current_row.chunk || current_row.chunk->size() == 0) {
					// exhausted all rows
					base_row = 0;
					result = nullptr;
					current_row.chunk.reset();
				}
			}
		}

		QueryResultIterator &operator++() {
			Next();
			return *this;
		}
		bool operator!=(const QueryResultIterator &other) const {
			return result != other.result || base_row != other.base_row || current_row.row != other.current_row.row;
		}
		bool operator==(const QueryResultIterator &other) const {
			return !(*this != other);
		}
		const QueryResultRow &operator*() const {
			return current_row;
		}
	};

public:
	using iterator = QueryResultIterator;

	iterator begin() { // NOLINT: match stl API
		return QueryResultIterator(this);
	}
	iterator end() { // NOLINT: match stl API
		return QueryResultIterator(nullptr);
	}

protected:
	GOOSE_API string HeaderToString();

private:
	QueryResult(const QueryResult &) = delete;
};

} // namespace goose
