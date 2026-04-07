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

namespace goose {

class BatchMemoryManager : public StateWithBlockableTasks {
public:
	BatchMemoryManager(ClientContext &context, idx_t initial_memory_request)
	    : context(context), unflushed_memory_usage(0), min_batch_index(0), available_memory(0),
	      can_increase_memory(true) {
		memory_state = TemporaryMemoryManager::Get(context).Register(context);
		SetMemorySize(initial_memory_request);
	}

private:
	ClientContext &context;
	//! Temporary memory state
	unique_ptr<TemporaryMemoryState> memory_state;
	//! Total memory usage of unflushed rows
	atomic<idx_t> unflushed_memory_usage;
	//! Minimum batch size
	atomic<idx_t> min_batch_index;
	//! The available memory for unflushed rows
	atomic<idx_t> available_memory;
	//! Whether or not we can request additional memory
	bool can_increase_memory;

public:
	void SetMemorySize(idx_t size) {
		// request at most 1/4th of all available memory
		idx_t total_max_memory = BufferManager::GetBufferManager(context).GetQueryMaxMemory();
		idx_t request_cap = total_max_memory / 4;

		size = MinValue<idx_t>(size, request_cap);
		if (size <= available_memory) {
			return;
		}

		memory_state->SetRemainingSizeAndUpdateReservation(context, size);
		auto next_reservation = memory_state->GetReservation();
		if (available_memory >= next_reservation) {
			// we tried to ask for more memory but were declined
			// stop asking for more memory
			can_increase_memory = false;
		}
		available_memory = next_reservation;
	}

	void IncreaseMemory() {
		if (!can_increase_memory) {
			return;
		}
		SetMemorySize(available_memory * 2);
	}

	bool OutOfMemory(idx_t batch_index) {
#ifdef GOOSE_ALTERNATIVE_VERIFY
		// alternative verify - always report that we are out of memory to test this code path
		return true;
#else
		if (unflushed_memory_usage >= available_memory) {
			auto guard = Lock();
			if (batch_index > min_batch_index) {
				// exceeded available memory and we are not the minimum batch index- try to increase it
				IncreaseMemory();
				if (unflushed_memory_usage >= available_memory) {
					// STILL out of memory
					return true;
				}
			}
		}
		return false;
#endif
	}

	void UpdateMinBatchIndex(idx_t current_min_batch_index) {
		if (min_batch_index >= current_min_batch_index) {
			return;
		}
		auto guard = Lock();
		auto new_batch_index = MaxValue<idx_t>(min_batch_index, current_min_batch_index);
		if (new_batch_index != min_batch_index) {
			// new batch index! unblock all tasks
			min_batch_index = new_batch_index;
			UnblockTasks(guard);
		}
	}

	idx_t AvailableMemory() const {
		return available_memory;
	}

	idx_t GetUnflushedMemory() {
		return unflushed_memory_usage;
	}

	void IncreaseUnflushedMemory(idx_t memory_increase) {
		unflushed_memory_usage += memory_increase;
	}

	void ReduceUnflushedMemory(idx_t memory_reduction) {
		if (unflushed_memory_usage < memory_reduction) {
			throw InternalException("Reducing unflushed memory usage below zero!?");
		} else {
			unflushed_memory_usage -= memory_reduction;
		}
	}

	bool IsMinimumBatchIndex(idx_t batch_index) {
		return batch_index <= min_batch_index;
	}

	void FinalCheck() {
		if (unflushed_memory_usage != 0) {
			throw InternalException("Unflushed memory usage is not zero at finalize but %llu",
			                        unflushed_memory_usage.load());
		}
	}
};

} // namespace goose
