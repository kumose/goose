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
//

#pragma once

#include <goose/common/enums/profiler_format.h>
#include <goose/common/serializer/buffered_file_writer.h>
#include <goose/common/winapi.h>
#include <goose/function/udf_function.h>
#include <goose/main/materialized_query_result.h>
#include <goose/main/pending_query_result.h>
#include <goose/main/prepared_statement.h>
#include <goose/main/query_result.h>
#include <goose/main/relation.h>
#include <goose/main/stream_query_result.h>
#include <goose/main/table_description.h>
#include <goose/parser/sql_statement.h>
#include <goose/main/profiling_node.h>

namespace goose {

class ColumnDataCollection;
class ClientContext;

class DatabaseInstance;
class Goose;
class LogicalOperator;
class SelectStatement;
struct CSVReaderOptions;

typedef void (*warning_callback_t)(std::string);

//! A connection to a database. This represents a (client) connection that can
//! be used to query the database.
class Connection {
public:
	GOOSE_API explicit Connection(Goose &database);
	GOOSE_API explicit Connection(DatabaseInstance &database);
	// disable copy constructors
	Connection(const Connection &other) = delete;
	Connection &operator=(const Connection &) = delete;
	//! enable move constructors
	GOOSE_API Connection(Connection &&other) noexcept;
	GOOSE_API Connection &operator=(Connection &&) noexcept;
	GOOSE_API ~Connection();

	shared_ptr<ClientContext> context;

public:
	//! Returns query profiling information for the current query
	GOOSE_API string GetProfilingInformation(ProfilerPrintFormat format = ProfilerPrintFormat::QUERY_TREE);

	//! Returns the first node of the query profiling tree
	GOOSE_API optional_ptr<ProfilingNode> GetProfilingTree();

	//! Interrupt execution of the current query
	GOOSE_API void Interrupt();

	//! Get query progress of current query
	GOOSE_API double GetQueryProgress();

	//! Enable query profiling
	GOOSE_API void EnableProfiling();
	//! Disable query profiling
	GOOSE_API void DisableProfiling();

	//! Enable aggressive verification/testing of queries, should only be used in testing
	GOOSE_API void EnableQueryVerification();
	GOOSE_API void DisableQueryVerification();
	//! Force parallel execution, even for smaller tables. Should only be used in testing.
	GOOSE_API void ForceParallelism();

	//! Issues a query to the database and returns a QueryResult. This result can be either a StreamQueryResult or a
	//! MaterializedQueryResult. The result can be stepped through with calls to Fetch(). Note that there can only be
	//! one active StreamQueryResult per Connection object. Calling SendQuery() will invalidate any previously existing
	//! StreamQueryResult.
	GOOSE_API unique_ptr<QueryResult>
	SendQuery(const string &query, QueryParameters query_parameters = QueryResultOutputType::ALLOW_STREAMING);
	GOOSE_API unique_ptr<QueryResult>
	SendQuery(unique_ptr<SQLStatement> statement,
	          QueryParameters query_parameters = QueryResultOutputType::ALLOW_STREAMING);
	//! Issues a query to the database and materializes the result (if necessary). Always returns a
	//! MaterializedQueryResult.
	GOOSE_API unique_ptr<MaterializedQueryResult> Query(const string &query);
	//! Issues a query to the database and materializes the result (if necessary). Always returns a
	//! MaterializedQueryResult.
	GOOSE_API unique_ptr<MaterializedQueryResult>
	Query(unique_ptr<SQLStatement> statement, QueryResultMemoryType memory_type = QueryResultMemoryType::IN_MEMORY);
	// prepared statements
	template <typename... ARGS>
	unique_ptr<QueryResult> Query(const string &query, ARGS... args) {
		vector<Value> values;
		return QueryParamsRecursive(query, values, args...);
	}

	//! Issues a query to the database and returns a Pending Query Result. Note that "query" may only contain
	//! a single statement.
	GOOSE_API unique_ptr<PendingQueryResult>
	PendingQuery(const string &query, QueryParameters query_parameters = QueryResultOutputType::FORCE_MATERIALIZED);
	//! Issues a query to the database and returns a Pending Query Result
	GOOSE_API unique_ptr<PendingQueryResult>
	PendingQuery(unique_ptr<SQLStatement> statement,
	             QueryParameters query_parameters = QueryResultOutputType::FORCE_MATERIALIZED);
	GOOSE_API unique_ptr<PendingQueryResult>
	PendingQuery(unique_ptr<SQLStatement> statement, case_insensitive_map_t<BoundParameterData> &named_values,
	             QueryParameters query_parameters = QueryResultOutputType::FORCE_MATERIALIZED);
	GOOSE_API unique_ptr<PendingQueryResult>
	PendingQuery(const string &query, case_insensitive_map_t<BoundParameterData> &named_values,
	             QueryParameters query_parameters = QueryResultOutputType::FORCE_MATERIALIZED);
	GOOSE_API unique_ptr<PendingQueryResult>
	PendingQuery(const string &query, vector<Value> &values,
	             QueryParameters query_parameters = QueryResultOutputType::FORCE_MATERIALIZED);
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(const string &query, PendingQueryParameters parameters);
	GOOSE_API unique_ptr<PendingQueryResult>
	PendingQuery(unique_ptr<SQLStatement> statement, vector<Value> &values,
	             QueryParameters query_parameters = QueryResultOutputType::FORCE_MATERIALIZED);

	//! Prepare the specified query, returning a prepared statement object
	GOOSE_API unique_ptr<PreparedStatement> Prepare(const string &query);
	//! Prepare the specified statement, returning a prepared statement object
	GOOSE_API unique_ptr<PreparedStatement> Prepare(unique_ptr<SQLStatement> statement);

	//! Get the table info of a specific table, or nullptr if it cannot be found.
	GOOSE_API unique_ptr<TableDescription> TableInfo(const string &database_name, const string &schema_name,
	                                                  const string &table_name);
	//! Get the table info of a specific table, or nullptr if it cannot be found. Uses INVALID_CATALOG.
	GOOSE_API unique_ptr<TableDescription> TableInfo(const string &schema_name, const string &table_name);
	//! Get the table info of a specific table, or nullptr if it cannot be found. Uses INVALID_CATALOG and
	//! DEFAULT_SCHEMA.
	GOOSE_API unique_ptr<TableDescription> TableInfo(const string &table_name);

	//! Extract a set of SQL statements from a specific query
	GOOSE_API vector<unique_ptr<SQLStatement>> ExtractStatements(const string &query);
	//! Extract the logical plan that corresponds to a query
	GOOSE_API unique_ptr<LogicalOperator> ExtractPlan(const string &query);

	//! Appends a DataChunk to the specified table
	GOOSE_API void Append(TableDescription &description, DataChunk &chunk);
	//! Appends a ColumnDataCollection to the specified table
	GOOSE_API void Append(TableDescription &description, ColumnDataCollection &collection);

	//! Returns a relation that produces a table from this connection
	GOOSE_API shared_ptr<Relation> Table(const string &tname);
	GOOSE_API shared_ptr<Relation> Table(const string &schema_name, const string &table_name);
	GOOSE_API shared_ptr<Relation> Table(const string &catalog_name, const string &schema_name,
	                                      const string &table_name);
	//! Returns a relation that produces a view from this connection
	GOOSE_API shared_ptr<Relation> View(const string &tname);
	GOOSE_API shared_ptr<Relation> View(const string &schema_name, const string &table_name);
	//! Returns a relation that calls a specified table function
	GOOSE_API shared_ptr<Relation> TableFunction(const string &tname);
	GOOSE_API shared_ptr<Relation> TableFunction(const string &tname, const vector<Value> &values,
	                                              const named_parameter_map_t &named_parameters);
	GOOSE_API shared_ptr<Relation> TableFunction(const string &tname, const vector<Value> &values);
	//! Returns a relation that produces values
	GOOSE_API shared_ptr<Relation> Values(const vector<vector<Value>> &values);
	GOOSE_API shared_ptr<Relation> Values(vector<vector<unique_ptr<ParsedExpression>>> &&values);
	GOOSE_API shared_ptr<Relation> Values(const vector<vector<Value>> &values, const vector<string> &column_names,
	                                       const string &alias = "values");
	GOOSE_API shared_ptr<Relation> Values(const string &values);
	GOOSE_API shared_ptr<Relation> Values(const string &values, const vector<string> &column_names,
	                                       const string &alias = "values");

	//! Reads CSV file
	GOOSE_API shared_ptr<Relation> ReadCSV(const string &csv_file);
	GOOSE_API shared_ptr<Relation> ReadCSV(const vector<string> &csv_input, named_parameter_map_t &&options);
	GOOSE_API shared_ptr<Relation> ReadCSV(const string &csv_input, named_parameter_map_t &&options);
	GOOSE_API shared_ptr<Relation> ReadCSV(const string &csv_file, const vector<string> &columns);

	//! Reads Parquet file
	GOOSE_API shared_ptr<Relation> ReadParquet(const string &parquet_file, bool binary_as_string);
	//! Returns a relation from a query
	GOOSE_API shared_ptr<Relation> RelationFromQuery(const string &query, const string &alias = "queryrelation",
	                                                  const string &error = "Expected a single SELECT statement");
	GOOSE_API shared_ptr<Relation> RelationFromQuery(unique_ptr<SelectStatement> select_stmt,
	                                                  const string &alias = "queryrelation", const string &query = "");

	GOOSE_API void BeginTransaction();
	GOOSE_API void Commit();
	GOOSE_API void Rollback();
	GOOSE_API void SetAutoCommit(bool auto_commit);
	GOOSE_API bool IsAutoCommit();
	GOOSE_API bool HasActiveTransaction();

	//! Fetch the set of tables names of the query.
	//! Returns the fully qualified, escaped table names, if qualified is set to true,
	//! else returns the not qualified, not escaped table names.
	GOOSE_API unordered_set<string> GetTableNames(const string &query, const bool qualified = false);

	// NOLINTBEGIN
	template <typename TR, typename... ARGS>
	void CreateScalarFunction(const string &name, TR (*udf_func)(ARGS...)) {
		scalar_function_t function = UDFWrapper::CreateScalarFunction<TR, ARGS...>(name, udf_func);
		UDFWrapper::RegisterFunction<TR, ARGS...>(name, function, *context);
	}

	template <typename TR, typename... ARGS>
	void CreateScalarFunction(const string &name, vector<LogicalType> args, LogicalType ret_type,
	                          TR (*udf_func)(ARGS...)) {
		scalar_function_t function = UDFWrapper::CreateScalarFunction<TR, ARGS...>(name, args, ret_type, udf_func);
		UDFWrapper::RegisterFunction(name, args, ret_type, function, *context);
	}

	template <typename TR, typename... ARGS>
	void CreateVectorizedFunction(const string &name, scalar_function_t udf_func,
	                              LogicalType varargs = LogicalType::INVALID) {
		UDFWrapper::RegisterFunction<TR, ARGS...>(name, udf_func, *context, std::move(varargs));
	}

	void CreateVectorizedFunction(const string &name, vector<LogicalType> args, LogicalType ret_type,
	                              scalar_function_t udf_func, LogicalType varargs = LogicalType::INVALID) {
		UDFWrapper::RegisterFunction(name, std::move(args), std::move(ret_type), std::move(udf_func), *context,
		                             std::move(varargs));
	}

	//------------------------------------- Aggreate Functions ----------------------------------------//
	template <typename UDF_OP, typename STATE, typename TR, typename TA>
	void CreateAggregateFunction(const string &name) {
		AggregateFunction function = UDFWrapper::CreateAggregateFunction<UDF_OP, STATE, TR, TA>(name);
		UDFWrapper::RegisterAggrFunction(function, *context);
	}

	template <typename UDF_OP, typename STATE, typename TR, typename TA, typename TB>
	void CreateAggregateFunction(const string &name) {
		AggregateFunction function = UDFWrapper::CreateAggregateFunction<UDF_OP, STATE, TR, TA, TB>(name);
		UDFWrapper::RegisterAggrFunction(function, *context);
	}

	template <typename UDF_OP, typename STATE, typename TR, typename TA>
	void CreateAggregateFunction(const string &name, LogicalType ret_type, LogicalType input_type_a) {
		AggregateFunction function =
		    UDFWrapper::CreateAggregateFunction<UDF_OP, STATE, TR, TA>(name, ret_type, input_type_a);
		UDFWrapper::RegisterAggrFunction(function, *context);
	}

	template <typename UDF_OP, typename STATE, typename TR, typename TA, typename TB>
	void CreateAggregateFunction(const string &name, LogicalType ret_type, LogicalType input_type_a,
	                             LogicalType input_type_b) {
		AggregateFunction function =
		    UDFWrapper::CreateAggregateFunction<UDF_OP, STATE, TR, TA, TB>(name, ret_type, input_type_a, input_type_b);
		UDFWrapper::RegisterAggrFunction(function, *context);
	}

	void CreateAggregateFunction(const string &name, const vector<LogicalType> &arguments,
	                             const LogicalType &return_type, aggregate_size_t state_size,
	                             aggregate_initialize_t initialize, aggregate_update_t update,
	                             aggregate_combine_t combine, aggregate_finalize_t finalize,
	                             aggregate_simple_update_t simple_update = nullptr,
	                             bind_aggregate_function_t bind = nullptr,
	                             aggregate_destructor_t destructor = nullptr) {
		AggregateFunction function =
		    UDFWrapper::CreateAggregateFunction(name, arguments, return_type, state_size, initialize, update, combine,
		                                        finalize, simple_update, bind, destructor);
		UDFWrapper::RegisterAggrFunction(function, *context);
	}
	// NOLINTEND

protected:
	//! Identified used to uniquely identify connections to the database.
	connection_t connection_id;

private:
	unique_ptr<QueryResult> QueryParamsRecursive(const string &query, vector<Value> &values);

	template <typename T, typename... ARGS>
	unique_ptr<QueryResult> QueryParamsRecursive(const string &query, vector<Value> &values, T value, ARGS... args) {
		values.push_back(Value::CreateValue<T>(value));
		return QueryParamsRecursive(query, values, args...);
	}
};

} // namespace goose
