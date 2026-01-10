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

#include <goose/parallel/task.h>
#include <goose/common/optional_ptr.h>

namespace goose {
class Event;
class PhysicalOperator;
class ThreadContext;

//! Execute a task within an executor, including exception handling
//! This should be used within queries
class ExecutorTask : public Task {
public:
	ExecutorTask(Executor &executor, shared_ptr<Event> event);
	ExecutorTask(ClientContext &context, shared_ptr<Event> event, const PhysicalOperator &op);
	~ExecutorTask() override;

public:
	void Deschedule() override;
	void Reschedule() override;

public:
	Executor &executor;
	shared_ptr<Event> event;
	unique_ptr<ThreadContext> thread_context;
	optional_ptr<const PhysicalOperator> op;

private:
	ClientContext &context;

public:
	virtual TaskExecutionResult ExecuteTask(TaskExecutionMode mode) = 0;
	TaskExecutionResult Execute(TaskExecutionMode mode) override;
};

} // namespace goose
