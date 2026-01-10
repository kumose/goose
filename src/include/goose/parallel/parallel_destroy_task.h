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
