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
#include <goose/storage/temporary_memory_manager.h>
#include <goose/parallel/interrupt.h>
#include <goose/storage/buffer_manager.h>
#include <goose/common/queue.h>

namespace goose {

template <class TASK>
class BatchTaskManager {
public:
	void AddTask(unique_ptr<TASK> task) {
		lock_guard<mutex> l(task_lock);
		task_queue.push(std::move(task));
	}

	unique_ptr<TASK> GetTask() {
		lock_guard<mutex> l(task_lock);
		if (task_queue.empty()) {
			return nullptr;
		}
		auto entry = std::move(task_queue.front());
		task_queue.pop();
		return entry;
	}

	idx_t TaskCount() {
		lock_guard<mutex> l(task_lock);
		return task_queue.size();
	}

private:
	mutex task_lock;
	//! The task queue for the batch copy to file
	queue<unique_ptr<TASK>> task_queue;
};

} // namespace goose
