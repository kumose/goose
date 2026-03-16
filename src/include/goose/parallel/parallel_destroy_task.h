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
#include <goose/parallel/task_executor.h>

namespace goose {
template <class ITERABLE>
class ParallelDestroyTask : public BaseExecutorTask {
	using VALUE_TYPE = typename ITERABLE::value_type;

public:
	ParallelDestroyTask(TaskExecutor &executor, VALUE_TYPE &&elem_p)
	    : BaseExecutorTask(executor), elem(std::move(elem_p)) {
	}

	void ExecuteTask() override {
		elem.reset();
	}

	static void Schedule(TaskScheduler &scheduler, ITERABLE &elements) {
		// This is used in destructors and is therefore not allowed to throw
		// Similar approach to AttachedDatabase::Close(), just swallow the exception
		try {
			TaskExecutor executor(scheduler);
			for (auto &segment : elements) {
				auto destroy_task = make_uniq<ParallelDestroyTask>(executor, std::move(segment));
				executor.ScheduleTask(std::move(destroy_task));
			}
			executor.WorkOnTasks();
		} catch (...) { // NOLINT
		}
	}

private:
	VALUE_TYPE elem;
};

} // namespace goose
