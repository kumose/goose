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

#include <goose/parallel/task_scheduler.h>

namespace goose {

class TaskCounter {
public:
	explicit TaskCounter(TaskScheduler &scheduler_p)
	    : scheduler(scheduler_p), token(scheduler_p.CreateProducer()), task_count(0), tasks_completed(0) {
	}

	virtual void AddTask(shared_ptr<Task> task) {
		++task_count;
		scheduler.ScheduleTask(*token, std::move(task));
	}

	virtual void FinishTask() const {
		++tasks_completed;
	}

	virtual void Finish() {
		while (tasks_completed < task_count) {
			shared_ptr<Task> task;
			if (scheduler.GetTaskFromProducer(*token, task)) {
				task->Execute();
				task.reset();
			}
		}
	}

private:
	TaskScheduler &scheduler;
	unique_ptr<ProducerToken> token;
	size_t task_count;
	mutable atomic<size_t> tasks_completed;
};

} // namespace goose
