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

#include <goose/execution/physical_operator_states.h>

namespace goose {

class Sort;
class TupleDataLayout;
struct BoundOrderByNode;
struct ProgressData;
class SortedRun;
enum class SortKeyType : uint8_t;
class TaskScheduler;

class SortedRunMerger {
	friend class SortedRunMergerLocalState;
	friend class SortedRunMergerGlobalState;

public:
	SortedRunMerger(const Sort &sort, vector<unique_ptr<SortedRun>> &&sorted_runs, idx_t partition_size, bool external,
	                bool is_index_sort);
	~SortedRunMerger();

public:
	//===--------------------------------------------------------------------===//
	// Source Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context, GlobalSourceState &gstate) const;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const;
	SourceResultType GetData(ExecutionContext &context, DataChunk &chunk, OperatorSourceInput &input) const;
	OperatorPartitionData GetPartitionData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
	                                       LocalSourceState &lstate, const OperatorPartitionInfo &partition_info) const;
	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const;

public:
	//===--------------------------------------------------------------------===//
	// Non-Standard Interface
	//===--------------------------------------------------------------------===//
	SourceResultType MaterializeSortedRun(ExecutionContext &context, OperatorSourceInput &input) const;
	unique_ptr<SortedRun> GetSortedRun(GlobalSourceState &global_state);

private:
	TaskScheduler &scheduler;

public:
	const Sort &sort;
	vector<unique_ptr<SortedRun>> sorted_runs;
	const idx_t total_count;

	const idx_t partition_size;
	const bool external;
	const bool is_index_sort;
};

} // namespace goose
