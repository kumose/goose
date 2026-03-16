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
#include <goose/common/deque.h>
#include <goose/common/vector_size.h>
#include <goose/common/types/data_chunk.h>
#include <goose/main/buffered_data/simple_buffered_data.h>
#include <goose/common/types-import.h>

namespace goose {

class StreamQueryResult;

class InProgressBatch {
public:
	//! The chunks that make up the batch
	deque<unique_ptr<DataChunk>> chunks;
	//! Whether the batch is completed (NextBatch has been called)
	bool completed = false;
};

class BatchedBufferedData : public BufferedData {
public:
	static constexpr const BufferedData::Type TYPE = BufferedData::Type::BATCHED;

public:
	explicit BatchedBufferedData(ClientContext &context);

public:
	void Append(const DataChunk &chunk, idx_t batch);
	void BlockSink(const InterruptState &blocked_sink, idx_t batch);

	bool ShouldBlockBatch(idx_t batch);
	StreamExecutionResult ExecuteTaskInternal(StreamQueryResult &result, ClientContextLock &context_lock) override;
	unique_ptr<DataChunk> Scan() override;
	void UpdateMinBatchIndex(idx_t min_batch_index);
	bool IsMinimumBatchIndex(lock_guard<mutex> &lock, idx_t batch);
	void CompleteBatch(idx_t batch);
	bool BufferIsEmpty();
	void UnblockSinks() override;

	inline idx_t ReadQueueCapacity() const {
		return read_queue_capacity;
	}
	inline idx_t BufferCapacity() const {
		return buffer_capacity;
	}

private:
	void ResetReplenishState();
	void MoveCompletedBatches(lock_guard<mutex> &lock);

private:
	//! The buffer where chunks are written before they are ready to be read.
	map<idx_t, InProgressBatch> buffer;
	idx_t buffer_capacity;
	atomic<idx_t> buffer_byte_count;

	//! The queue containing the chunks that can be read.
	deque<unique_ptr<DataChunk>> read_queue;
	idx_t read_queue_capacity;
	atomic<idx_t> read_queue_byte_count;

	map<idx_t, InterruptState> blocked_sinks;

	idx_t min_batch;
	//! Debug variable to verify that order is preserved correctly.
	idx_t lowest_moved_batch = 0;
};

} // namespace goose
