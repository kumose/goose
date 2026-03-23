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
#include <goose/execution/progress_data.h>
#include <goose/common/sorting/sort_projection_column.h>
#include <goose/planner/bound_result_modifier.h>

namespace goose {

class SortLocalSinkState;
class SortGlobalSinkState;

class SortLocalSourceState;
class SortGlobalSourceState;

class SortedRun;
class SortedRunScanState;

class SortedRunMerger;
class SortedRunMergerLocalState;
class SortedRunMergerGlobalState;

class TupleDataLayout;
class ColumnDataCollection;

//! Class that sorts the data, follows the PhysicalOperator interface
class Sort {
	friend class SortLocalSinkState;
	friend class SortGlobalSinkState;

	friend class SortLocalSourceState;
	friend class SortGlobalSourceState;

	friend class SortedRun;
	friend class SortedRunScanState;

	friend class SortedRunMerger;
	friend class SortedRunMergerLocalState;
	friend class SortedRunMergerGlobalState;

public:
	Sort(ClientContext &context, const vector<BoundOrderByNode> &orders, const vector<LogicalType> &input_types,
	     vector<idx_t> projection_map, bool is_index_sort = false);

public:
	//! The client context
	ClientContext &context;

private:
	//! Key orders, expressions, and layout
	unique_ptr<Expression> create_sort_key;
	unique_ptr<Expression> decode_sort_key;
	shared_ptr<TupleDataLayout> key_layout;

	//! Projection map and payload layout (columns that also appear as key eliminated)
	vector<idx_t> payload_projection_map;
	shared_ptr<TupleDataLayout> payload_layout;

	//! Mapping from key/payload layouts to output columns
	vector<idx_t> input_projection_map;
	vector<SortProjectionColumn> output_projection_columns;

	//! Whether to force an approximate sort
	bool is_index_sort;

public:
	//===--------------------------------------------------------------------===//
	// Sink Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &context) const;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const;
	SinkFinalizeType Finalize(ClientContext &context, OperatorSinkFinalizeInput &input) const;
	ProgressData GetSinkProgress(ClientContext &context, GlobalSinkState &gstate,
	                             const ProgressData source_progress) const;

public:
	//===--------------------------------------------------------------------===//
	// Source Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context, GlobalSourceState &gstate) const;
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context, GlobalSinkState &sink) const;
	SourceResultType GetData(ExecutionContext &context, DataChunk &chunk, OperatorSourceInput &input) const;
	OperatorPartitionData GetPartitionData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
	                                       LocalSourceState &lstate, const OperatorPartitionInfo &partition_info) const;
	ProgressData GetProgress(ClientContext &context, GlobalSourceState &gstate) const;

public:
	//===--------------------------------------------------------------------===//
	// Non-Standard Interface
	//===--------------------------------------------------------------------===//
	SourceResultType MaterializeColumnData(ExecutionContext &context, OperatorSourceInput &input) const;
	unique_ptr<ColumnDataCollection> GetColumnData(OperatorSourceInput &input) const;

	SourceResultType MaterializeSortedRun(ExecutionContext &context, OperatorSourceInput &input) const;
	unique_ptr<SortedRun> GetSortedRun(GlobalSourceState &global_state);
};

} // namespace goose
