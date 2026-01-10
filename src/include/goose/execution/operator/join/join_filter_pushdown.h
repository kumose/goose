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

#include <goose/planner/column_binding.h>
#include <goose/planner/expression.h>
#include <goose/planner/table_filter.h>

namespace goose {
class DataChunk;
class DynamicTableFilterSet;
class LogicalGet;
class JoinHashTable;
class PhysicalComparisonJoin;
struct GlobalUngroupedAggregateState;
struct LocalUngroupedAggregateState;

struct JoinFilterPushdownColumn {
	//! The probe column index to which this filter should be applied
	ColumnBinding probe_column_index;
};

struct JoinFilterGlobalState {
	~JoinFilterGlobalState();

	//! Global Min/Max aggregates for filter pushdown
	unique_ptr<GlobalUngroupedAggregateState> global_aggregate_state;
};

struct JoinFilterLocalState {
	~JoinFilterLocalState();

	//! Local Min/Max aggregates for filter pushdown
	unique_ptr<LocalUngroupedAggregateState> local_aggregate_state;
};

struct JoinFilterPushdownFilter {
	//! The dynamic table filter set where to push filters into
	shared_ptr<DynamicTableFilterSet> dynamic_filters;
	//! The columns for which we should generate filters
	vector<JoinFilterPushdownColumn> columns;
};

struct PushdownFilterTarget {
	PushdownFilterTarget(LogicalGet &get, vector<JoinFilterPushdownColumn> columns_p)
	    : get(get), columns(std::move(columns_p)) {
	}

	LogicalGet &get;
	vector<JoinFilterPushdownColumn> columns;
};

struct JoinFilterPushdownInfo {
	//! The join condition indexes for which we compute the min/max aggregates
	vector<idx_t> join_condition;
	//! The probes to push the filter into
	vector<JoinFilterPushdownFilter> probe_info;
	//! Min/Max aggregates
	vector<unique_ptr<Expression>> min_max_aggregates;

	//! Whether the build side has a filter -> we might be able to push down a bloom filter into the probe side
	bool build_side_has_filter;

public:
	unique_ptr<JoinFilterGlobalState> GetGlobalState(ClientContext &context, const PhysicalOperator &op) const;
	unique_ptr<JoinFilterLocalState> GetLocalState(JoinFilterGlobalState &gstate) const;

	void Sink(DataChunk &chunk, JoinFilterLocalState &lstate) const;
	void Combine(JoinFilterGlobalState &gstate, JoinFilterLocalState &lstate) const;
	unique_ptr<DataChunk> Finalize(ClientContext &context, optional_ptr<JoinHashTable> ht,
	                               JoinFilterGlobalState &gstate, const PhysicalComparisonJoin &op) const;

	unique_ptr<DataChunk> FinalizeMinMax(JoinFilterGlobalState &gstate) const;
	unique_ptr<DataChunk> FinalizeFilters(ClientContext &context, optional_ptr<JoinHashTable> ht,
	                                      const PhysicalComparisonJoin &op, unique_ptr<DataChunk> final_min_max,
	                                      bool is_perfect_hashtable) const;

private:
	void PushInFilter(const JoinFilterPushdownFilter &info, JoinHashTable &ht, const PhysicalOperator &op,
	                  idx_t filter_idx, idx_t filter_col_idx) const;

	void PushBloomFilter(const JoinFilterPushdownFilter &info, JoinHashTable &ht, const PhysicalOperator &op,
	                     idx_t filter_col_idx) const;

	bool CanUseInFilter(const ClientContext &context, optional_ptr<JoinHashTable> ht, const ExpressionType &cmp) const;
	bool CanUseBloomFilter(const ClientContext &context, optional_ptr<JoinHashTable> ht,
	                       const PhysicalComparisonJoin &op, const ExpressionType &cmp,
	                       bool is_perfect_hashtable) const;
};

} // namespace goose
