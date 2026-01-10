// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/common/types-import.h>
#include <goose/common/deque.h>
#include <goose/common/enums/pending_execution_result.h>
#include <goose/common/enums/prepared_statement_mode.h>
#include <goose/common/error_data.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/winapi.h>
#include <goose/main/client_config.h>
#include <goose/main/client_context_state.h>
#include <goose/main/client_properties.h>
#include <goose/main/external_dependencies.h>
#include <goose/main/pending_query_result.h>
#include <goose/main/prepared_statement.h>
#include <goose/main/stream_query_result.h>
#include <goose/main/table_description.h>
#include <goose/planner/expression/bound_parameter_data.h>
#include <goose/transaction/transaction_context.h>
#include <goose/main/query_parameters.h>

namespace goose {

class Appender;
class Catalog;
class CatalogSearchPath;
class ColumnDataCollection;
class DatabaseInstance;
class FileOpener;
class LogicalOperator;
class PreparedStatementData;
class Relation;
class BufferedFileWriter;
class QueryProfiler;
class ClientContextLock;
struct CreateScalarFunctionInfo;
class ScalarFunctionCatalogEntry;
struct ActiveQueryContext;
struct ParserOptions;
class SimpleBufferedData;
class BufferedData;
struct ClientData;
class ClientContextState;
class RegisteredStateManager;

struct PendingQueryParameters {
	//! Prepared statement parameters (if any)
	optional_ptr<case_insensitive_map_t<BoundParameterData>> parameters;
	//! Whether a stream/buffer-managed result should be allowed
	QueryParameters query_parameters;
};

//! The ClientContext holds information relevant to the current client session
//! during execution
class ClientContext : public enable_shared_from_this<ClientContext> {
	friend class PendingQueryResult;  // LockContext
	friend class BufferedData;        // ExecuteTaskInternal
	friend class SimpleBufferedData;  // ExecuteTaskInternal
	friend class BatchedBufferedData; // ExecuteTaskInternal
	friend class StreamQueryResult;   // LockContext
	friend class ConnectionManager;

public:
	GOOSE_API explicit ClientContext(shared_ptr<DatabaseInstance> db);
	GOOSE_API ~ClientContext();

	//! The database that this client is connected to
	shared_ptr<DatabaseInstance> db;
	//! Whether or not the query is interrupted
	atomic<bool> interrupted;
	//! Set of optional states (e.g. Caches) that can be held by the ClientContext
	unique_ptr<RegisteredStateManager> registered_state;
	//! The logger to be used by this ClientContext
	shared_ptr<Logger> logger;
	//! The client configuration
	ClientConfig config;
	//! The set of client-specific data
	unique_ptr<ClientData> client_data;
	//! Data for the currently running transaction
	TransactionContext transaction;

public:
	MetaTransaction &ActiveTransaction() {
		return transaction.ActiveTransaction();
	}

	//! Interrupt execution of a query
	GOOSE_API void Interrupt();
	GOOSE_API bool IsInterrupted() const;
	GOOSE_API void ClearInterrupt();
	GOOSE_API void CancelTransaction();

	//! Enable query profiling
	GOOSE_API void EnableProfiling();
	//! Disable query profiling
	GOOSE_API void DisableProfiling();

	//! Issue a query, returning a QueryResult. The QueryResult can be either a StreamQueryResult or a
	//! MaterializedQueryResult. The StreamQueryResult will only be returned in the case of a successful SELECT
	//! statement.
	GOOSE_API unique_ptr<QueryResult> Query(const string &query, QueryParameters query_parameters);
	GOOSE_API unique_ptr<QueryResult> Query(unique_ptr<SQLStatement> statement, QueryParameters query_parameters);

	//! Issues a query to the database and returns a Pending Query Result. Note that "query" may only contain
	//! a single statement.
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(const string &query, QueryParameters query_parameters);
	//! Issues a query to the database and returns a Pending Query Result
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(unique_ptr<SQLStatement> statement,
	                                                       QueryParameters query_parameters);

	//! Create a pending query with a list of parameters
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(unique_ptr<SQLStatement> statement,
	                                                       case_insensitive_map_t<BoundParameterData> &values,
	                                                       QueryParameters query_parameters);
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(const string &query,
	                                                       case_insensitive_map_t<BoundParameterData> &values,
	                                                       QueryParameters query_parameters);
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(const string &query, PendingQueryParameters parameters);

	//! Destroy the client context
	GOOSE_API void Destroy();

	//! Get the table info of a specific table, or nullptr if it cannot be found.
	GOOSE_API unique_ptr<TableDescription> TableInfo(const string &database_name, const string &schema_name,
	                                                  const string &table_name);
	//! Get the table info of a specific table, or nullptr if it cannot be found. Uses INVALID_CATALOG.
	GOOSE_API unique_ptr<TableDescription> TableInfo(const string &schema_name, const string &table_name);
	//! Execute a query with the given collection "attached" to the query using a CTE
	GOOSE_API void Append(ColumnDataCollection &collection, const string &query, const vector<string> &column_names,
	                       const string &collection_name);
	//! Appends a DataChunk and its default columns to the specified table.
	GOOSE_API void Append(TableDescription &description, ColumnDataCollection &collection,
	                       optional_ptr<const vector<LogicalIndex>> column_ids = nullptr);

	//! Try to bind a relation in the current client context; either throws an exception or fills the result_columns
	//! list with the set of returned columns
	GOOSE_API void TryBindRelation(Relation &relation, vector<ColumnDefinition> &result_columns);

	//! Internal function for try bind relation. It does not require a client-context lock.
	GOOSE_API void InternalTryBindRelation(Relation &relation, vector<ColumnDefinition> &result_columns);

	//! Execute a relation
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(const shared_ptr<Relation> &relation,
	                                                       QueryParameters query_parameters);
	GOOSE_API unique_ptr<QueryResult> Execute(const shared_ptr<Relation> &relation);

	//! Prepare a query
	GOOSE_API unique_ptr<PreparedStatement> Prepare(const string &query);
	//! Directly prepare a SQL statement
	GOOSE_API unique_ptr<PreparedStatement> Prepare(unique_ptr<SQLStatement> statement);

	//! Create a pending query result from a prepared statement with the given name and set of parameters
	//! It is possible that the prepared statement will be re-bound. This will generally happen if the catalog is
	//! modified in between the prepared statement being bound and the prepared statement being run.
	GOOSE_API unique_ptr<PendingQueryResult> PendingQuery(const string &query,
	                                                       shared_ptr<PreparedStatementData> &prepared,
	                                                       const PendingQueryParameters &parameters);

	//! Execute a prepared statement with the given name and set of parameters
	//! It is possible that the prepared statement will be re-bound. This will generally happen if the catalog is
	//! modified in between the prepared statement being bound and the prepared statement being run.
	GOOSE_API unique_ptr<QueryResult>
	Execute(const string &query, shared_ptr<PreparedStatementData> &prepared,
	        case_insensitive_map_t<BoundParameterData> &values,
	        QueryParameters query_parameters = QueryResultOutputType::ALLOW_STREAMING);
	GOOSE_API unique_ptr<QueryResult> Execute(const string &query, shared_ptr<PreparedStatementData> &prepared,
	                                           const PendingQueryParameters &parameters);

	//! Gets current percentage of the query's progress, returns 0 in case the progress bar is disabled.
	GOOSE_API QueryProgress GetQueryProgress();

	//! Register function in the temporary schema
	GOOSE_API void RegisterFunction(CreateFunctionInfo &info);

	//! Parse statements from a query
	GOOSE_API vector<unique_ptr<SQLStatement>> ParseStatements(const string &query);

	//! Extract the logical plan of a query
	GOOSE_API unique_ptr<LogicalOperator> ExtractPlan(const string &query);
	GOOSE_API void HandlePragmaStatements(vector<unique_ptr<SQLStatement>> &statements);

	//! Runs a function with a valid transaction context, potentially starting a transaction if the context is in auto
	//! commit mode.
	GOOSE_API void RunFunctionInTransaction(const std::function<void(void)> &fun,
	                                         bool requires_valid_transaction = true);
	//! Same as RunFunctionInTransaction, but does not obtain a lock on the client context or check for validation
	GOOSE_API void RunFunctionInTransactionInternal(ClientContextLock &lock, const std::function<void(void)> &fun,
	                                                 bool requires_valid_transaction = true);

	//! Equivalent to CURRENT_SETTING(key) SQL function.
	GOOSE_API SettingLookupResult TryGetCurrentSetting(const string &key, Value &result) const;

	//! Returns the parser options for this client context
	GOOSE_API ParserOptions GetParserOptions() const;

	//! Whether or not the given result object (streaming query result or pending query result) is active
	GOOSE_API bool IsActiveResult(ClientContextLock &lock, BaseQueryResult &result);

	//! Returns the current executor
	Executor &GetExecutor();

	//! Return the current logger
	Logger &GetLogger() const;

	//! Returns the current query string (if any)
	const string &GetCurrentQuery();

	connection_t GetConnectionId() const;

	//! Fetch the set of tables names of the query.
	//! Returns the fully qualified, escaped table names, if qualified is set to true,
	//! else returns the not qualified, not escaped table names.
	GOOSE_API unordered_set<string> GetTableNames(const string &query, const bool qualified = false);

	GOOSE_API ClientProperties GetClientProperties();

	//! Returns true if execution of the current query is finished
	GOOSE_API bool ExecutionIsFinished();

	//! Process an error for display to the user
	GOOSE_API void ProcessError(ErrorData &error, const string &query) const;

private:
	//! Parse statements and resolve pragmas from a query
	vector<unique_ptr<SQLStatement>> ParseStatements(ClientContextLock &lock, const string &query);
	//! Issues a query to the database and returns a Pending Query Result
	unique_ptr<PendingQueryResult> PendingQueryInternal(ClientContextLock &lock, unique_ptr<SQLStatement> statement,
	                                                    const PendingQueryParameters &parameters, bool verify = true);
	unique_ptr<QueryResult> ExecutePendingQueryInternal(ClientContextLock &lock, PendingQueryResult &query);

	//! Parse statements from a query
	vector<unique_ptr<SQLStatement>> ParseStatementsInternal(ClientContextLock &lock, const string &query);
	//! Perform aggressive query verification of a SELECT statement. Only called when query_verification_enabled is
	//! true.
	ErrorData VerifyQuery(ClientContextLock &lock, const string &query, unique_ptr<SQLStatement> statement,
	                      PendingQueryParameters parameters);

	void InitialCleanup(ClientContextLock &lock);
	//! Internal clean up, does not lock. Caller must hold the context_lock.
	void CleanupInternal(ClientContextLock &lock, BaseQueryResult *result = nullptr,
	                     bool invalidate_transaction = false);
	unique_ptr<PendingQueryResult> PendingStatementOrPreparedStatement(ClientContextLock &lock, const string &query,
	                                                                   unique_ptr<SQLStatement> statement,
	                                                                   shared_ptr<PreparedStatementData> &prepared,
	                                                                   const PendingQueryParameters &parameters);
	unique_ptr<PendingQueryResult> PendingPreparedStatement(ClientContextLock &lock, const string &query,
	                                                        shared_ptr<PreparedStatementData> statement_p,
	                                                        const PendingQueryParameters &parameters);
	unique_ptr<PendingQueryResult> PendingPreparedStatementInternal(ClientContextLock &lock,
	                                                                shared_ptr<PreparedStatementData> statement_data_p,
	                                                                const PendingQueryParameters &parameters);
	void CheckIfPreparedStatementIsExecutable(PreparedStatementData &statement);

	//! Internally prepare a SQL statement. Caller must hold the context_lock.
	shared_ptr<PreparedStatementData>
	CreatePreparedStatement(ClientContextLock &lock, const string &query, unique_ptr<SQLStatement> statement,
	                        PendingQueryParameters parameters,
	                        PreparedStatementMode mode = PreparedStatementMode::PREPARE_ONLY);
	unique_ptr<PendingQueryResult> PendingStatementInternal(ClientContextLock &lock, const string &query,
	                                                        unique_ptr<SQLStatement> statement,
	                                                        const PendingQueryParameters &parameters);
	unique_ptr<QueryResult> RunStatementInternal(ClientContextLock &lock, const string &query,
	                                             unique_ptr<SQLStatement> statement,
	                                             const PendingQueryParameters &parameters, bool verify = true);
	unique_ptr<PreparedStatement> PrepareInternal(ClientContextLock &lock, unique_ptr<SQLStatement> statement);
	void LogQueryInternal(ClientContextLock &lock, const string &query);

	unique_ptr<QueryResult> FetchResultInternal(ClientContextLock &lock, PendingQueryResult &pending);

	unique_ptr<ClientContextLock> LockContext();

	void BeginQueryInternal(ClientContextLock &lock, const string &query);
	ErrorData EndQueryInternal(ClientContextLock &lock, bool success, bool invalidate_transaction,
	                           optional_ptr<ErrorData> previous_error);

	//! Wait until a task is available to execute
	void WaitForTask(ClientContextLock &lock, BaseQueryResult &result);
	PendingExecutionResult ExecuteTaskInternal(ClientContextLock &lock, BaseQueryResult &result, bool dry_run = false);

	unique_ptr<PendingQueryResult> PendingStatementOrPreparedStatementInternal(
	    ClientContextLock &lock, const string &query, unique_ptr<SQLStatement> statement,
	    shared_ptr<PreparedStatementData> &prepared, const PendingQueryParameters &parameters);

	unique_ptr<PendingQueryResult> PendingQueryPreparedInternal(ClientContextLock &lock, const string &query,
	                                                            shared_ptr<PreparedStatementData> &prepared,
	                                                            const PendingQueryParameters &parameters);

	unique_ptr<PendingQueryResult> PendingQueryInternal(ClientContextLock &, const shared_ptr<Relation> &relation,
	                                                    QueryParameters query_parameters);

	void RebindPreparedStatement(ClientContextLock &lock, const string &query,
	                             shared_ptr<PreparedStatementData> &prepared, const PendingQueryParameters &parameters);

	template <class T>
	unique_ptr<T> ErrorResult(ErrorData error, const string &query = string());

	shared_ptr<PreparedStatementData> CreatePreparedStatementInternal(ClientContextLock &lock, const string &query,
	                                                                  unique_ptr<SQLStatement> statement,
	                                                                  PendingQueryParameters parameters);

	SettingLookupResult TryGetCurrentSettingInternal(const string &key, Value &result) const;

private:
	//! Lock on using the ClientContext in parallel
	mutex context_lock;
	//! The currently active query context
	unique_ptr<ActiveQueryContext> active_query;
	//! The current query progress
	QueryProgress query_progress;
	//! The connection corresponding to this client context
	connection_t connection_id;
};

class ClientContextLock {
public:
	explicit ClientContextLock(mutex &context_lock) : client_guard(context_lock) {
	}

	~ClientContextLock() {
	}

private:
	lock_guard<mutex> client_guard;
};

//! The QueryContext wraps an optional client context.
//! It makes query-related information available to operations.
class QueryContext {
public:
	QueryContext() : context(nullptr) {
	}
	QueryContext(optional_ptr<ClientContext> context) : context(context) { // NOLINT: allow implicit construction
	}
	QueryContext(ClientContext &context) : context(&context) { // NOLINT: allow implicit construction
	}

public:
	bool Valid() const {
		return context != nullptr;
	}
	optional_ptr<ClientContext> GetClientContext() const {
		return context;
	}

private:
	optional_ptr<ClientContext> context;
};

} // namespace goose
