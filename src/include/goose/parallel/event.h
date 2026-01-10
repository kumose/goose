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

#include <goose/execution/executor.h>
#include <goose/common/types-import.h>
#include <goose/common/common.h>
#include <goose/common/vector.h>

namespace goose {
class Executor;
class Task;

class Event : public enable_shared_from_this<Event> {
public:
	explicit Event(Executor &executor);
	virtual ~Event() = default;

public:
	virtual void Schedule() = 0;
	//! Called right after the event is finished
	virtual void FinishEvent() {
	}
	//! Called after the event is entirely finished
	virtual void FinalizeFinish() {
	}

	void FinishTask();
	void Finish();

	void AddDependency(Event &event);
	bool HasDependencies() const {
		return total_dependencies != 0;
	}
	const vector<reference<Event>> &GetParentsVerification() const;

	void CompleteDependency();

	void SetTasks(vector<shared_ptr<Task>> tasks);

	void InsertEvent(shared_ptr<Event> replacement_event);

	bool IsFinished() const {
		return finished;
	}

	virtual void PrintPipeline() {
	}

	ClientContext &GetClientContext();

	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}

protected:
	Executor &executor;
	//! The current threads working on the event
	atomic<idx_t> finished_tasks;
	//! The maximum amount of threads that can work on the event
	atomic<idx_t> total_tasks;

	//! The amount of completed dependencies
	//! The event can only be started after the dependencies have finished executing
	atomic<idx_t> finished_dependencies;
	//! The total amount of dependencies
	idx_t total_dependencies;

	//! The events that depend on this event to run
	vector<weak_ptr<Event>> parents;
	//! Raw pointers to the parents (used for verification only)
	vector<reference<Event>> parents_raw;

	//! Whether or not the event is finished executing
	atomic<bool> finished;
};

} // namespace goose
