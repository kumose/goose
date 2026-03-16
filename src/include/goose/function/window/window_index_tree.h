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
