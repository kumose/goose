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
