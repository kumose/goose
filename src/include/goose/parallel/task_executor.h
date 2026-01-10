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
