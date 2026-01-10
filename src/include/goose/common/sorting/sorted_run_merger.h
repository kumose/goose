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
