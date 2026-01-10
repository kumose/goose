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

class FullSort : public SortStrategy {
public:
	using Orders = vector<BoundOrderByNode>;

	FullSort(ClientContext &client, const vector<BoundOrderByNode> &order_bys, const Types &payload_types,
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

public:
	//===--------------------------------------------------------------------===//
	// Source Interface
	//===--------------------------------------------------------------------===//
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context, GlobalSinkState &sink) const override;

public:
	//===--------------------------------------------------------------------===//
	// Non-Standard Interface
	//===--------------------------------------------------------------------===//
	SourceResultType MaterializeColumnData(ExecutionContext &context, idx_t hash_bin,
	                                       OperatorSourceInput &source) const override;
	HashGroupPtr GetColumnData(idx_t hash_bin, OperatorSourceInput &source) const override;

	SourceResultType MaterializeSortedRun(ExecutionContext &context, idx_t hash_bin,
	                                      OperatorSourceInput &source) const override;
	SortedRunPtr GetSortedRun(ClientContext &client, idx_t hash_bin, OperatorSourceInput &source) const override;

	const ChunkRows &GetHashGroups(GlobalSourceState &global_state) const override;

public:
	// OVER(...) (sorting)
	Orders orders;
	//! Are we creating a dummy payload column?
	bool force_payload = false;
	// Key columns that must be computed
	vector<unique_ptr<Expression>> sort_exprs;
	//! Common sort description
	unique_ptr<Sort> sort;

private:
	SourceResultType MaterializeSortedData(ExecutionContext &context, bool build_runs,
	                                       OperatorSourceInput &source) const;
};

} // namespace goose
