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
