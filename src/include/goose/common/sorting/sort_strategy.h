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
