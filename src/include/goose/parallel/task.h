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
#include <goose/common/optional_ptr.h>

namespace goose {
class ClientContext;
class Executor;
class Task;
class DatabaseInstance;
struct ProducerToken;

enum class TaskExecutionMode : uint8_t { PROCESS_ALL, PROCESS_PARTIAL };

enum class TaskExecutionResult : uint8_t { TASK_FINISHED, TASK_NOT_FINISHED, TASK_ERROR, TASK_BLOCKED };

//! Generic parallel task
class Task : public enable_shared_from_this<Task> {
public:
	virtual ~Task() {
	}

public:
	//! Execute the task in the specified execution mode
	//! If mode is PROCESS_ALL, Execute should always finish processing and return TASK_FINISHED
	//! If mode is PROCESS_PARTIAL, Execute can return TASK_NOT_FINISHED, in which case Execute will be called again
	//! In case of an error, TASK_ERROR is returned
	//! In case the task has interrupted, BLOCKED is returned.
	virtual TaskExecutionResult Execute(TaskExecutionMode mode) = 0;

	//! Descheduling a task ensures the task is not executed, but remains available for rescheduling as long as
	//! required, generally until some code in an operator calls the InterruptState::Callback() method of a state of the
	//! InterruptMode::TASK mode.
	virtual void Deschedule() {
		throw InternalException("Cannot deschedule task of base Task class");
	};

	//! Ensures a task is rescheduled to the correct queue
	virtual void Reschedule() {
		throw InternalException("Cannot reschedule task of base Task class");
	}

	virtual bool TaskBlockedOnResult() const {
		return false;
	}

	virtual string TaskType() const {
		return "UnnamedTask";
	}

public:
	optional_ptr<ProducerToken> token;
};

} // namespace goose
