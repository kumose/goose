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
