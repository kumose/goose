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
