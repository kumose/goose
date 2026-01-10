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
