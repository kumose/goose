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

class WindowIndexTree;

class WindowIndexTreeLocalState : public WindowMergeSortTreeLocalState {
public:
	WindowIndexTreeLocalState(ExecutionContext &context, WindowIndexTree &index_tree);

	//! Process sorted leaf data
	void BuildLeaves() override;

	//! The index tree we are building
	WindowIndexTree &index_tree;
};

class WindowIndexTree : public WindowMergeSortTree {
public:
	WindowIndexTree(ClientContext &context, const vector<BoundOrderByNode> &orders, const vector<column_t> &sort_idx,
	                const idx_t count);
	WindowIndexTree(ClientContext &context, const BoundOrderModifier &order_bys, const vector<column_t> &sort_idx,
	                const idx_t count);
	~WindowIndexTree() override = default;

	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context) override;

	//! Find the Nth index in the set of subframes
	//! Returns {nth index, 0} or {nth offset, overflow}
	pair<idx_t, idx_t> SelectNth(const SubFrames &frames, idx_t n) const;
};

} // namespace goose
