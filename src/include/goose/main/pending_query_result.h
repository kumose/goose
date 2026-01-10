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

#include <goose/main/query_result.h>
#include <goose/common/enums/pending_execution_result.h>
#include <goose/execution/executor.h>

namespace goose {
class ClientContext;
class ClientContextLock;
class PreparedStatementData;

class PendingQueryResult : public BaseQueryResult {
	friend class ClientContext;

public:
	static constexpr const QueryResultType TYPE = QueryResultType::PENDING_RESULT;

public:
	GOOSE_API PendingQueryResult(shared_ptr<ClientContext> context, PreparedStatementData &statement,
	                              vector<LogicalType> types, bool allow_stream_result);
	GOOSE_API explicit PendingQueryResult(ErrorData error_message);
	GOOSE_API ~PendingQueryResult() override;
	GOOSE_API bool AllowStreamResult() const;
	PendingQueryResult(const PendingQueryResult &) = delete;
	PendingQueryResult &operator=(const PendingQueryResult &) = delete;

public:
	//! Executes a single task within the query, returning whether or not the query is ready.
	//! If this returns RESULT_READY, the Execute function can be called to obtain a pointer to the result.
	//! If this returns RESULT_NOT_READY, the ExecuteTask function should be called again.
	//! If this returns EXECUTION_ERROR, an error occurred during execution.
	//! If this returns NO_TASKS_AVAILABLE, this means currently no meaningful work can be done by the current executor,
	//!	    but tasks may become available in the future.
	//! The error message can be obtained by calling GetError() on the PendingQueryResult.
	GOOSE_API PendingExecutionResult ExecuteTask();
	GOOSE_API PendingExecutionResult CheckPulse();
	//! Halt execution of the thread until a Task is ready to be executed (use with caution)
	void WaitForTask();

	//! Returns the result of the query as an actual query result.
	//! This returns (mostly) instantly if ExecuteTask has been called until RESULT_READY was returned.
	GOOSE_API unique_ptr<QueryResult> Execute();

	GOOSE_API void Close();

	//! Function to determine whether execution is considered finished
	GOOSE_API static bool IsResultReady(PendingExecutionResult result);
	GOOSE_API static bool IsExecutionFinished(PendingExecutionResult result);

private:
	shared_ptr<ClientContext> context;
	bool allow_stream_result;

private:
	void CheckExecutableInternal(ClientContextLock &lock);

	PendingExecutionResult ExecuteTaskInternal(ClientContextLock &lock);
	unique_ptr<QueryResult> ExecuteInternal(ClientContextLock &lock);
	unique_ptr<ClientContextLock> LockContext();
};

} // namespace goose
