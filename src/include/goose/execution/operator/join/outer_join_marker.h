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
