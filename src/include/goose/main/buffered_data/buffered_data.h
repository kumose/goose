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
