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

#include <goose/parallel/interrupt.h>
#include <goose/common/queue.h>
#include <goose/common/vector_size.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/optional_idx.h>
#include <goose/execution/physical_operator_states.h>
#include <goose/common/enums/pending_execution_result.h>
#include <goose/common/enums/stream_execution_result.h>
#include <goose/common/shared_ptr.h>

namespace goose {

class StreamQueryResult;
class ClientContextLock;

class BufferedData {
protected:
	enum class Type { SIMPLE, BATCHED };

public:
	BufferedData(Type type, ClientContext &context);
	virtual ~BufferedData();

public:
	StreamExecutionResult ReplenishBuffer(StreamQueryResult &result, ClientContextLock &context_lock);
	virtual StreamExecutionResult ExecuteTaskInternal(StreamQueryResult &result, ClientContextLock &context_lock) = 0;
	virtual unique_ptr<DataChunk> Scan() = 0;
	virtual void UnblockSinks() = 0;
	shared_ptr<ClientContext> GetContext() {
		return context.lock();
	}
	bool Closed() const {
		if (context.expired()) {
			return true;
		}
		auto c = context.lock();
		return c == nullptr;
	}
	void Close() {
		context.reset();
	}

public:
	template <class TARGET>
	TARGET &Cast() {
		if (TARGET::TYPE != type) {
			throw InternalException("Failed to cast buffered data to type - buffered data type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (TARGET::TYPE != type) {
			throw InternalException("Failed to cast buffered data to type - buffered data type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}

protected:
	Type type;
	//! This is weak to avoid a cyclical reference
	weak_ptr<ClientContext> context;
	//! The maximum amount of memory we should keep buffered
	idx_t total_buffer_size;
	//! Protect against populate/fetch race condition
	mutex glock;
};

} // namespace goose
