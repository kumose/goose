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

#include <goose/execution/operator/helper/physical_result_collector.h>
#include <goose/common/queue.h>

namespace goose {

class BufferedBatchCollectorLocalState : public LocalSinkState {
public:
	BufferedBatchCollectorLocalState();

public:
	idx_t current_batch = 0;
};

class PhysicalBufferedBatchCollector : public PhysicalResultCollector {
public:
	PhysicalBufferedBatchCollector(PhysicalPlan &physical_plan, PreparedStatementData &data);

public:
	unique_ptr<QueryResult> GetResult(GlobalSinkState &state) const override;

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkNextBatchType NextBatch(ExecutionContext &context, OperatorSinkNextBatchInput &input) const override;

	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	OperatorPartitionInfo RequiredPartitionInfo() const override {
		return OperatorPartitionInfo::BatchIndex();
	}

	bool ParallelSink() const override {
		return true;
	}

	bool IsStreaming() const override {
		return true;
	}
};

} // namespace goose
