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
