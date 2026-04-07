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

#include <goose/common/types-import.h>
#include <goose/common/types/column/column_data_collection.h>
#include <goose/execution/operator/join/physical_comparison_join.h>
#include <goose/execution/physical_operator.h>

namespace goose {

struct OuterJoinGlobalScanState {
	mutex lock;
	ColumnDataCollection *data = nullptr;
	ColumnDataParallelScanState global_scan;
};

struct OuterJoinLocalScanState {
	DataChunk scan_chunk;
	SelectionVector match_sel;
	ColumnDataLocalScanState local_scan;
};

class OuterJoinMarker {
public:
	explicit OuterJoinMarker(bool enabled);

	bool Enabled() const {
		return enabled;
	}
	//! Initializes the outer join counter
	void Initialize(idx_t count);
	//! Resets the outer join counter
	void Reset();

	//! Sets an indiivdual match
	void SetMatch(idx_t position);

	//! Sets multiple matches
	void SetMatches(const SelectionVector &sel, idx_t count, idx_t base_idx = 0);

	//! Constructs a left-join result based on which tuples have not found matches
	void ConstructLeftJoinResult(DataChunk &left, DataChunk &result);

	//! Returns the maximum number of threads that can be associated with an right-outer join scan
	idx_t MaxThreads() const;

	//! Initialize a scan
	void InitializeScan(ColumnDataCollection &data, OuterJoinGlobalScanState &gstate);

	//! Initialize a local scan
	void InitializeScan(OuterJoinGlobalScanState &gstate, OuterJoinLocalScanState &lstate);

	//! Perform the scan
	void Scan(OuterJoinGlobalScanState &gstate, OuterJoinLocalScanState &lstate, DataChunk &result);

	//! Read-only matches vector
	const bool *GetMatches() const {
		return found_match.get();
	}

private:
	bool enabled;
	unsafe_unique_array<bool> found_match;
	idx_t count;
};

} // namespace goose
