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

#include <goose/common/enums/copy_overwrite_mode.h>
#include <goose/common/file_system.h>
#include <goose/common/filename_pattern.h>
#include <goose/execution/physical_operator.h>
#include <goose/function/copy_function.h>
#include <goose/parser/parsed_data/copy_info.h>
#include <goose/storage/storage_lock.h>

namespace goose {

struct CopyToFileInfo {
	explicit CopyToFileInfo(string file_path_p) : file_path(std::move(file_path_p)) {
	}

	string file_path;
	unique_ptr<CopyFunctionFileStatistics> file_stats;
	Value partition_keys;
};

//! Copy the contents of a query into a table
class PhysicalCopyToFile : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::COPY_TO_FILE;

public:
	PhysicalCopyToFile(PhysicalPlan &physical_plan, vector<LogicalType> types, CopyFunction function,
	                   unique_ptr<FunctionData> bind_data, idx_t estimated_cardinality);

	CopyFunction function;
	unique_ptr<FunctionData> bind_data;
	string file_path;
	bool use_tmp_file;
	FilenamePattern filename_pattern;
	string file_extension;
	CopyOverwriteMode overwrite_mode;
	bool parallel;
	bool per_thread_output;
	optional_idx file_size_bytes;
	bool rotate;
	CopyFunctionReturnType return_type;

	bool partition_output;
	bool write_partition_columns;
	bool write_empty_file;
	bool hive_file_pattern;
	vector<idx_t> partition_columns;
	vector<string> names;
	vector<LogicalType> expected_types;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
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
	SinkFinalizeType FinalizeInternal(ClientContext &context, GlobalSinkState &gstate) const;
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const override;

	bool IsSink() const override {
		return true;
	}

	bool SinkOrderDependent() const override {
		return true;
	}

	bool ParallelSink() const override {
		return per_thread_output || partition_output || parallel;
	}

public:
	static void MoveTmpFile(ClientContext &context, const string &tmp_file_path);
	static string GetNonTmpFile(ClientContext &context, const string &tmp_file_path);

	string GetTrimmedPath(ClientContext &context) const;

	static void ReturnStatistics(DataChunk &chunk, idx_t row_idx, CopyToFileInfo &written_file_info);

private:
	unique_ptr<GlobalFunctionData> CreateFileState(ClientContext &context, GlobalSinkState &sink,
	                                               StorageLockKey &global_lock) const;
	void WriteRotateInternal(ExecutionContext &context, GlobalSinkState &global_state,
	                         const std::function<void(GlobalFunctionData &)> &fun) const;
};
} // namespace goose
