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

#include <goose/common/common.h>
#include <goose/common/types-import.h>
#include <goose/common/optional_ptr.h>
#include <goose/parallel/task.h>
#include <goose/execution/task_error_manager.h>

namespace goose {
class TaskScheduler;

//! The TaskExecutor is a helper class that enables parallel scheduling and execution of tasks
class TaskExecutor {
public:
	explicit TaskExecutor(ClientContext &context);
	explicit TaskExecutor(TaskScheduler &scheduler);
	~TaskExecutor();

	//! Push an error into the TaskExecutor
	void PushError(ErrorData error);
	//! Whether or not any task has encountered an error
	bool HasError();
	//! Throw an error that was encountered during execution (if HasError() is true)
	void ThrowError();

	//! Schedule a new task
	void ScheduleTask(unique_ptr<Task> task);
	//! Label a task as finished
	void FinishTask();

	//! Work on tasks until all tasks are finished. Throws an exception if any error occurred while executing the tasks.
	void WorkOnTasks();

	//! Get a task - returns true if a task was found
	bool GetTask(shared_ptr<Task> &task);

private:
	TaskScheduler &scheduler;
	TaskErrorManager error_manager;
	unique_ptr<ProducerToken> token;
	atomic<idx_t> completed_tasks;
	atomic<idx_t> total_tasks;
	friend class BaseExecutorTask;
	optional_ptr<ClientContext> context;
};

class BaseExecutorTask : public Task {
public:
	explicit BaseExecutorTask(TaskExecutor &executor);

	virtual void ExecuteTask() = 0;
	TaskExecutionResult Execute(TaskExecutionMode mode) override;

protected:
	TaskExecutor &executor;
};

} // namespace goose
