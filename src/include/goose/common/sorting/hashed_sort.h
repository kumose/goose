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

#include <goose/common/sorting/sort_strategy.h>

namespace goose {

class HashedSort : public SortStrategy {
public:
	using Orders = vector<BoundOrderByNode>;
	using Types = vector<LogicalType>;
	using HashGroupPtr = unique_ptr<ColumnDataCollection>;
	using SortedRunPtr = unique_ptr<SortedRun>;

	static void GenerateOrderings(Orders &partitions, Orders &orders,
	                              const vector<unique_ptr<Expression>> &partition_bys, const Orders &order_bys,
	                              const vector<unique_ptr<BaseStatistics>> &partitions_stats);

	HashedSort(ClientContext &context, const vector<unique_ptr<Expression>> &partition_bys,
	           const vector<BoundOrderByNode> &order_bys, const Types &payload_types,
	           const vector<unique_ptr<BaseStatistics>> &partitions_stats, idx_t estimated_cardinality,
	           bool require_payload = false);

public:
	//===--------------------------------------------------------------------===//
	// Sink Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<LocalSinkState> GetLocalSinkState(ExecutionContext &context) const override;
	unique_ptr<GlobalSinkState> GetGlobalSinkState(ClientContext &client) const override;
	SinkResultType Sink(ExecutionContext &context, DataChunk &chunk, OperatorSinkInput &input) const override;
	SinkCombineResultType Combine(ExecutionContext &context, OperatorSinkCombineInput &input) const override;
	SinkFinalizeType Finalize(ClientContext &client, OperatorSinkFinalizeInput &finalize) const override;
	ProgressData GetSinkProgress(ClientContext &context, GlobalSinkState &gstate,
	                             const ProgressData source_progress) const override;
	void Synchronize(const GlobalSinkState &source, GlobalSinkState &target) const override;

public:
	//===--------------------------------------------------------------------===//
	// Source Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context, GlobalSinkState &sink) const override;

public:
	//===--------------------------------------------------------------------===//
	// Non-Standard Interface
	//===--------------------------------------------------------------------===//
	void SortColumnData(ExecutionContext &context, hash_t hash_bin, OperatorSinkFinalizeInput &finalize) override;

	SourceResultType MaterializeColumnData(ExecutionContext &context, idx_t hash_bin,
	                                       OperatorSourceInput &source) const override;
	HashGroupPtr GetColumnData(idx_t hash_bin, OperatorSourceInput &source) const override;

	SourceResultType MaterializeSortedRun(ExecutionContext &context, idx_t hash_bin,
	                                      OperatorSourceInput &source) const override;
	SortedRunPtr GetSortedRun(ClientContext &client, idx_t hash_bin, OperatorSourceInput &source) const override;

	const ChunkRows &GetHashGroups(GlobalSourceState &global_state) const override;

public:
	//! The host's estimated row count
	const idx_t estimated_cardinality;

	//! The PARTITION BY sorting
	Orders partitions;
	//! The ORDER BY sorting
	Orders orders;
	//! The partition columns
	vector<column_t> partition_ids;
	//! Are we creating a dummy payload column?
	bool force_payload = false;
	// Key columns that must be computed
	vector<unique_ptr<Expression>> sort_exprs;
	//! Common sort description
	unique_ptr<Sort> sort;
};

} // namespace goose
