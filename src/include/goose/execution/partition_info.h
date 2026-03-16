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

#include <goose/common/common.h>
#include <goose/common/types/value.h>
#include <goose/common/optional_idx.h>

namespace goose {

enum class PartitionInfo { NONE, REQUIRES_BATCH_INDEX };

struct ColumnPartitionData {
	explicit ColumnPartitionData(Value partition_val) : min_val(partition_val), max_val(std::move(partition_val)) {
	}

	Value min_val;
	Value max_val;
};

struct SourcePartitionInfo {
	//! The current batch index
	//! This is only set in case RequiresBatchIndex() is true, and the source has support for it (SupportsBatchIndex())
	//! Otherwise this is left on INVALID_INDEX
	//! The batch index is a globally unique, increasing index that should be used to maintain insertion order
	//! //! in conjunction with parallelism
	optional_idx batch_index;
	//! The minimum batch index that any thread is currently actively reading
	optional_idx min_batch_index;
	//! Column partition data
	vector<ColumnPartitionData> partition_data;
};

struct OperatorPartitionInfo {
	OperatorPartitionInfo() = default;
	explicit OperatorPartitionInfo(bool batch_index) : batch_index(batch_index) {
	}
	explicit OperatorPartitionInfo(vector<column_t> partition_columns_p)
	    : partition_columns(std::move(partition_columns_p)) {
	}

	bool batch_index = false;
	vector<column_t> partition_columns;

	static OperatorPartitionInfo NoPartitionInfo() {
		return OperatorPartitionInfo(false);
	}
	static OperatorPartitionInfo BatchIndex() {
		return OperatorPartitionInfo(true);
	}
	static OperatorPartitionInfo PartitionColumns(vector<column_t> columns) {
		return OperatorPartitionInfo(std::move(columns));
	}
	bool RequiresPartitionColumns() const {
		return !partition_columns.empty();
	}
	bool RequiresBatchIndex() const {
		return batch_index;
	}
	bool AnyRequired() const {
		return RequiresPartitionColumns() || RequiresBatchIndex();
	}
};

struct OperatorPartitionData {
	explicit OperatorPartitionData(idx_t batch_index) : batch_index(batch_index) {
	}

	idx_t batch_index;
	vector<ColumnPartitionData> partition_data;
};

} // namespace goose
