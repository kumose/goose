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

#include <goose/function/window/window_merge_sort_tree.h>

namespace goose {

// Builds a merge sort tree that uses integer tokens for the comparison values instead of the sort keys.
class WindowTokenTree : public WindowMergeSortTree {
public:
	WindowTokenTree(ClientContext &context, const vector<BoundOrderByNode> &orders, const vector<column_t> &order_idx,
	                const idx_t count, bool unique = false)
	    : WindowMergeSortTree(context, orders, order_idx, count, unique) {
	}
	WindowTokenTree(ClientContext &context, const BoundOrderModifier &order_bys, const vector<column_t> &order_idx,
	                const idx_t count, bool unique = false)
	    : WindowTokenTree(context, order_bys.orders, order_idx, count, unique) {
	}

	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context) override;

	//! Thread-safe post-sort cleanup
	void Finished() override;

	//! Find the rank of the row within the range
	idx_t Rank(const idx_t lower, const idx_t upper, const idx_t row_idx) const;

	//! Find the next peer after the row and within the range
	idx_t PeerEnd(const idx_t lower, const idx_t upper, const idx_t row_idx) const;

	//! Peer boundaries.
	vector<uint8_t> deltas;

protected:
	//! Find the starts of all the blocks
	idx_t MeasurePayloadBlocks() override;
};

} // namespace goose
