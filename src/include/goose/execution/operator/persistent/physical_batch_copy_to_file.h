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

#include <goose/common/file_system.h>
#include <goose/common/filename_pattern.h>
#include <goose/execution/physical_operator.h>
#include <goose/function/copy_function.h>
#include <goose/parser/parsed_data/copy_info.h>

namespace goose {
struct FixedRawBatchData;

class PhysicalBatchCopyToFile : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::BATCH_COPY_TO_FILE;

public:
	PhysicalBatchCopyToFile(PhysicalPlan &physical_plan, vector<LogicalType> types, CopyFunction function,
	                        unique_ptr<FunctionData> bind_data, idx_t estimated_cardinality);

	CopyFunction function;
	unique_ptr<FunctionData> bind_data;
	string file_path;
	bool use_tmp_file;
	CopyFunctionReturnType return_type;
	bool write_empty_file;

public:
	// Source interface
	SourceResultType GetDataInternal(ExecutionContext &context, DataChunk &chunk,
	                                 OperatorSourceInput &input) const override;

	bool IsSource() const override {
		return true;
	}

public:
	// Sink interface
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
	                          OperatorSinkFinalizeInput &input) const override;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;
	SinkNextBatchType NextBatch(ExecutionContext &context, OperatorSinkNextBatchInput &input) const override;

	OperatorPartitionInfo RequiredPartitionInfo() const override {
		return OperatorPartitionInfo::BatchIndex();
	}

	bool IsSink() const override {
		return true;
	}

	bool ParallelSink() const override {
		return true;
	}

public:
	void AddLocalBatch(ClientContext &context, GlobalSinkState &gstate, LocalSinkState &state) const;
	void AddRawBatchData(ClientContext &context, GlobalSinkState &gstate_p, idx_t batch_index,
	                     unique_ptr<FixedRawBatchData> collection) const;
	void RepartitionBatches(ClientContext &context, GlobalSinkState &gstate_p, idx_t min_index,
	                        bool final = false) const;
	void FlushBatchData(ClientContext &context, GlobalSinkState &gstate_p) const;
	bool ExecuteTask(ClientContext &context, GlobalSinkState &gstate_p) const;
	void ExecuteTasks(ClientContext &context, GlobalSinkState &gstate_p) const;
	SinkFinalizeType FinalFlush(ClientContext &context, GlobalSinkState &gstate_p) const;
};
} // namespace goose
