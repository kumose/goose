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

#include <goose/execution/operator/aggregate/aggregate_object.h>

namespace goose {

struct WindowAggregateStates {
	WindowAggregateStates(ClientContext &client, const AggregateObject &aggr);
	~WindowAggregateStates() {
		Destroy();
	}

	//! The number of states
	idx_t GetCount() const {
		return states.size() / state_size;
	}
	data_ptr_t *GetData() {
		return FlatVector::GetData<data_ptr_t>(*statef);
	}
	data_ptr_t GetStatePtr(idx_t idx) {
		return states.data() + idx * state_size;
	}
	const_data_ptr_t GetStatePtr(idx_t idx) const {
		return states.data() + idx * state_size;
	}
	//! Initialise all the states
	void Initialize(idx_t count);
	//! Combine the states into the target
	void Combine(WindowAggregateStates &target);
	//! Finalize the states into an output vector
	void Finalize(Vector &result);
	//! Destroy the states
	void Destroy();

	//! The context to use for memory etc.
	ClientContext &client;
	//! A description of the aggregator
	const AggregateObject aggr;
	//! The size of each state
	const idx_t state_size;
	//! The allocator to use
	ArenaAllocator allocator;
	//! Data pointer that contains the state data
	vector<data_t> states;
	//! Reused result state container for the window functions
	unique_ptr<Vector> statef;
};

} // namespace goose
