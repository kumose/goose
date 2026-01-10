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

#include <goose/common/row_operations/row_operations.h>
#include <goose/execution/execution_context.h>
#include <goose/execution/join_hashtable.h>
#include <goose/execution/physical_operator.h>

namespace goose {

class HashJoinOperatorState;
class HashJoinGlobalSinkState;
class PhysicalHashJoin;

struct PerfectHashJoinStats {
	Value build_min;
	Value build_max;
	bool is_build_small = false;
	bool is_build_dense = false;
	idx_t build_range = 0;
};

//! PhysicalHashJoin represents a hash loop join between two tables
class PerfectHashJoinExecutor {
	using PerfectHashTable = vector<buffer_ptr<VectorChildBuffer>>;

public:
	PerfectHashJoinExecutor(const PhysicalHashJoin &join, JoinHashTable &ht);

public:
	bool CanDoPerfectHashJoin(const PhysicalHashJoin &op, const Value &min, const Value &max);

	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context);
	OperatorResultType ProbePerfectHashTable(ExecutionContext &context, DataChunk &input, DataChunk &lhs_output_columns,
	                                         DataChunk &chunk, OperatorState &state);
	bool BuildPerfectHashTable(LogicalType &type);

private:
	void FillSelectionVectorSwitchProbe(Vector &source, SelectionVector &build_sel_vec, SelectionVector &probe_sel_vec,
	                                    idx_t count, idx_t &probe_sel_count);
	template <typename T>
	void TemplatedFillSelectionVectorProbe(Vector &source, SelectionVector &build_sel_vec,
	                                       SelectionVector &probe_sel_vec, idx_t count, idx_t &prob_sel_count);

	bool FillSelectionVectorSwitchBuild(Vector &source, SelectionVector &sel_vec, SelectionVector &seq_sel_vec,
	                                    idx_t count);
	template <typename T>
	bool TemplatedFillSelectionVectorBuild(Vector &source, SelectionVector &sel_vec, SelectionVector &seq_sel_vec,
	                                       idx_t count);
	bool FullScanHashTable(LogicalType &key_type);

private:
	const PhysicalHashJoin &join;
	JoinHashTable &ht;
	//! Columnar perfect hash table
	PerfectHashTable perfect_hash_table;
	//! Build statistics
	PerfectHashJoinStats perfect_join_statistics;
	//! Stores the occurrences of each value in the build side
	ValidityMask bitmap_build_idx;
	//! Stores the number of unique keys in the build side
	idx_t unique_keys = 0;
};

} // namespace goose
