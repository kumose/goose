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
