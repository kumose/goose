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

#include <goose/main/buffered_data/buffered_data.h>
#include <goose/parallel/interrupt.h>
#include <goose/common/queue.h>
#include <goose/common/vector_size.h>
#include <goose/common/types/data_chunk.h>

namespace goose {

class StreamQueryResult;
class ClientContextLock;

class SimpleBufferedData : public BufferedData {
public:
	static constexpr const BufferedData::Type TYPE = BufferedData::Type::SIMPLE;

public:
	explicit SimpleBufferedData(ClientContext &context);
	~SimpleBufferedData() override;

public:
	void Append(const DataChunk &chunk);
	void BlockSink(const InterruptState &blocked_sink);
	bool BufferIsFull();
	void UnblockSinks() override;
	StreamExecutionResult ExecuteTaskInternal(StreamQueryResult &result, ClientContextLock &context_lock) override;
	unique_ptr<DataChunk> Scan() override;
	inline idx_t BufferSize() const {
		return buffer_size;
	}

private:
	//! Our handles to reschedule the blocked sink tasks
	queue<InterruptState> blocked_sinks;
	//! The queue of chunks
	queue<unique_ptr<DataChunk>> buffered_chunks;
	//! The current capacity of the buffer (tuples)
	atomic<idx_t> buffered_count;
	//! The amount of tuples we should buffer
	idx_t buffer_size;
};

} // namespace goose
