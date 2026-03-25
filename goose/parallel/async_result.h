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

#include <goose/common/enum_util.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/common/enums/operator_result_type.h>
#include <goose/version.h>

namespace goose {

class InterruptState;
class TaskExecutor;
class Executor;

enum class AsyncResultsExecutionMode : uint8_t {
	SYNCHRONOUS,  // BLOCKED should not bubble up, and they should be executed synchronously
	TASK_EXECUTOR // BLOCKED is allowed
};

class AsyncTask {
public:
	virtual ~AsyncTask() {};
	virtual void Execute() = 0;
};

class AsyncResult {
	explicit AsyncResult(AsyncResultType t);

public:
	AsyncResult() = default;
	AsyncResult(AsyncResult &&) = default;
	AsyncResult(SourceResultType t); // NOLINT
	explicit AsyncResult(vector<unique_ptr<AsyncTask>> &&task);
	AsyncResult &operator=(SourceResultType t);
	AsyncResult &operator=(AsyncResultType t);
	AsyncResult &operator=(AsyncResult &&) noexcept;
	// Schedule held async_tasks into the Executor, eventually unblocking InterruptState
	// needs to be called with non-emopty async_tasks and from BLOCKED state, will empty the async_tasks and transform
	// into INVALID
	void ScheduleTasks(InterruptState &interrupt_state, Executor &executor);
	// Execute tasks synchronously at callsite
	// needs to be called with non-emopty async_tasks and from BLOCKED state, will empty the async_tasks and transform
	// into HAVE_MORE_OUTPUT
	void ExecuteTasksSynchronously();

	static AsyncResultType GetAsyncResultType(SourceResultType s);

	// Check whether there are tasks associated
	bool HasTasks() const;
	AsyncResultType GetResultType() const;
	// Extract associated tasks, moving them away, will empty async_tasks and trasnform to INVALID
	vector<unique_ptr<AsyncTask>> &&ExtractAsyncTasks();

#ifdef GOOSE_DEBUG_ASYNC_SINK_SOURCE
	static vector<unique_ptr<AsyncTask>> GenerateTestTasks();
#endif

	static AsyncResultsExecutionMode
	ConvertToAsyncResultExecutionMode(const PhysicalTableScanExecutionStrategy &execution_mode);

private:
	AsyncResultType result_type {AsyncResultType::INVALID};
	vector<unique_ptr<AsyncTask>> async_tasks {};
};
} // namespace goose
