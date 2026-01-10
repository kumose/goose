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

#include <goose/common/enum_util.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/common/enums/operator_result_type.h>

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
