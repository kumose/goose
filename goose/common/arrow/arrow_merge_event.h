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

#include <goose/parallel/base_pipeline_event.h>
#include <goose/parallel/task.h>
#include <goose/common/types/batched_data_collection.h>
#include <goose/parallel/pipeline.h>
#include <goose/common/vector.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/arrow/arrow_converter.h>
#include <goose/storage/buffer_manager.h>
#include <goose/main/chunk_scan_state/batched_data_collection.h>
#include <goose/execution/executor.h>
#include <goose/main/client_context.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/helper.h>
#include <goose/common/arrow/arrow_query_result.h>

namespace goose {

// Task to create one RecordBatch by (partially) scanning a BatchedDataCollection
class ArrowBatchTask : public ExecutorTask {
public:
	ArrowBatchTask(ArrowQueryResult &result, vector<idx_t> record_batch_indices, Executor &executor,
	               shared_ptr<Event> event_p, BatchCollectionChunkScanState scan_state, vector<string> names,
	               idx_t batch_size);
	void ProduceRecordBatches();
	TaskExecutionResult ExecuteTask(TaskExecutionMode mode) override;

	string TaskType() const override {
		return "ArrowBatchTask";
	}

private:
	ArrowQueryResult &result;
	vector<idx_t> record_batch_indices;
	shared_ptr<Event> event;
	idx_t batch_size;
	vector<string> names;
	BatchCollectionChunkScanState scan_state;
};

class ArrowMergeEvent : public BasePipelineEvent {
public:
	ArrowMergeEvent(ArrowQueryResult &result, BatchedDataCollection &batches, Pipeline &pipeline_p);

public:
	void Schedule() override;

public:
	ArrowQueryResult &result;
	BatchedDataCollection &batches;

private:
	//! The max size of a record batch to output
	idx_t record_batch_size;
};

} // namespace goose
