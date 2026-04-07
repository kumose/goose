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

#include <goose/common/winapi.h>
#include <goose/main/query_result.h>
#include <goose/parallel/interrupt.h>
#include <goose/common/queue.h>
#include <goose/common/enums/stream_execution_result.h>
#include <goose/main/buffered_data/simple_buffered_data.h>

namespace goose {

class ClientContext;
class ClientContextLock;
class Executor;
class MaterializedQueryResult;
class PreparedStatementData;

class StreamQueryResult : public QueryResult {
	friend class ClientContext;

public:
	static constexpr const QueryResultType TYPE = QueryResultType::STREAM_RESULT;

public:
	//! Create a successful StreamQueryResult. StreamQueryResults should always be successful initially (it makes no
	//! sense to stream an error).
	GOOSE_API StreamQueryResult(StatementType statement_type, StatementProperties properties,
	                             vector<LogicalType> types, vector<string> names, ClientProperties client_properties,
	                             shared_ptr<BufferedData> buffered_data);
	GOOSE_API explicit StreamQueryResult(ErrorData error);
	GOOSE_API ~StreamQueryResult() override;

public:
	static bool IsChunkReady(StreamExecutionResult result);
	//! Reschedules the tasks that work on producing a result chunk, returning when at least one task can be executed
	GOOSE_API void WaitForTask();
	//! Executes a single task within the final pipeline, returning whether or not a chunk is ready to be fetched
	GOOSE_API StreamExecutionResult ExecuteTask();
	//! Converts the QueryResult to a string
	GOOSE_API string ToString() override;
	//! Materializes the query result and turns it into a materialized query result
	GOOSE_API unique_ptr<MaterializedQueryResult> Materialize();

	GOOSE_API bool IsOpen();

	//! Closes the StreamQueryResult
	GOOSE_API void Close();

	//! The client context this StreamQueryResult belongs to
	shared_ptr<ClientContext> context;

protected:
	GOOSE_API unique_ptr<DataChunk> FetchInternal() override;

private:
	StreamExecutionResult ExecuteTaskInternal(ClientContextLock &lock);
	unique_ptr<DataChunk> FetchNextInternal(ClientContextLock &lock);
	unique_ptr<ClientContextLock> LockContext();
	void CheckExecutableInternal(ClientContextLock &lock);
	bool IsOpenInternal(ClientContextLock &lock);

private:
	shared_ptr<BufferedData> buffered_data;
};

} // namespace goose
