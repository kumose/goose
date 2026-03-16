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

#include <goose/common/sorting/sort.h>

namespace goose {

class SortStrategy {
public:
	using Types = vector<LogicalType>;
	using HashGroupPtr = unique_ptr<ColumnDataCollection>;
	using SortedRunPtr = unique_ptr<SortedRun>;

	static unique_ptr<SortStrategy> Factory(ClientContext &context, const vector<unique_ptr<Expression>> &partition_bys,
	                                        const vector<BoundOrderByNode> &order_bys, const Types &payload_types,
	                                        const vector<unique_ptr<BaseStatistics>> &partitions_stats,
	                                        idx_t estimated_cardinality, bool require_payload = false);

	explicit SortStrategy(const Types &input_types);
	virtual ~SortStrategy() = default;

public:
	//===--------------------------------------------------------------------===//
	// Sink Interface
	//===--------------------------------------------------------------------===//
	virtual unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const = 0;
	virtual unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &client) const = 0;
	virtual SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const = 0;
	virtual SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const = 0;
	virtual SinkFinalizeType Finalize(ClientContext &client, OperatorSinkFinalizeInput &finalize) const = 0;
	virtual ProgressData GetSinkProgress(ClientContext &context, GlobalSinkState &gstate,
	                                     const ProgressData source_progress) const = 0;
	virtual void Synchronize(const GlobalSinkState &source, GlobalSinkState &target) const;

public:
	//===--------------------------------------------------------------------===//
	// Source Interface
	//===--------------------------------------------------------------------===//
	virtual unique_ptr<LocalSourceState> GetLocalSourceState(ExecutionContext &context,
	                                                         GlobalSourceState &gstate) const;
	virtual unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context, GlobalSinkState &sink) const = 0;

public:
	//===--------------------------------------------------------------------===//
	// Non-Standard Interface
	//===--------------------------------------------------------------------===//
	virtual void SortColumnData(ExecutionContext &context, hash_t hash_bin, OperatorSinkFinalizeInput &finalize);

	virtual SourceResultType MaterializeColumnData(ExecutionContext &context, idx_t hash_bin,
	                                               OperatorSourceInput &source) const = 0;
	virtual HashGroupPtr GetColumnData(idx_t hash_bin, OperatorSourceInput &source) const = 0;

	virtual SourceResultType MaterializeSortedRun(ExecutionContext &context, idx_t hash_bin,
	                                              OperatorSourceInput &source) const = 0;
	virtual SortedRunPtr GetSortedRun(ClientContext &client, idx_t hash_bin, OperatorSourceInput &source) const = 0;

	// The chunk and row counts of the hash groups.
	struct ChunkRow {
		idx_t chunks = 0;
		idx_t count = 0;
	};
	using ChunkRows = vector<ChunkRow>;
	virtual const ChunkRows &GetHashGroups(GlobalSourceState &global_state) const = 0;

public:
	//! The inserted data schema
	Types payload_types;
	//! Input columns in the sorted output
	vector<column_t> scan_ids;
	// Key columns in the sorted output. Needed for prefix computations.
	vector<column_t> sort_ids;
};

} // namespace goose
