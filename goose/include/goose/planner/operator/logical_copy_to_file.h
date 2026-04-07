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

#include <goose/common/enums/copy_overwrite_mode.h>
#include <goose/common/filename_pattern.h>
#include <goose/common/local_file_system.h>
#include <goose/common/optional_idx.h>
#include <goose/function/copy_function.h>
#include <goose/planner/logical_operator.h>
#include <goose/common/enums/preserve_order.h>

namespace goose {

class LogicalCopyToFile : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_COPY_TO_FILE;

public:
	LogicalCopyToFile(CopyFunction function, unique_ptr<FunctionData> bind_data, unique_ptr<CopyInfo> copy_info)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_COPY_TO_FILE), function(std::move(function)),
	      bind_data(std::move(bind_data)), copy_info(std::move(copy_info)) {
	}
	CopyFunction function;
	unique_ptr<FunctionData> bind_data;
	unique_ptr<CopyInfo> copy_info;

	std::string file_path;
	bool use_tmp_file;
	FilenamePattern filename_pattern;
	string file_extension;
	CopyOverwriteMode overwrite_mode;
	bool per_thread_output;
	optional_idx file_size_bytes;
	bool rotate;
	CopyFunctionReturnType return_type;

	bool partition_output;
	bool write_partition_columns;
	bool write_empty_file = true;
	bool hive_file_pattern = true;
	PreserveOrderType preserve_order = PreserveOrderType::AUTOMATIC;
	vector<idx_t> partition_columns;
	vector<string> names;
	vector<LogicalType> expected_types;

public:
	vector<ColumnBinding> GetColumnBindings() override;
	idx_t EstimateCardinality(ClientContext &context) override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	static vector<LogicalType> GetTypesWithoutPartitions(const vector<LogicalType> &col_types,
	                                                     const vector<idx_t> &part_cols, bool write_part_cols);
	static vector<string> GetNamesWithoutPartitions(const vector<string> &col_names, const vector<column_t> &part_cols,
	                                                bool write_part_cols);

protected:
	void ResolveTypes() override {
		types = GetCopyFunctionReturnLogicalTypes(return_type);
	}
};
} // namespace goose
