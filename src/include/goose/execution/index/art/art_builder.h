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

#include <goose/execution/index/art/art.h>
#include <goose/common/types-import.h>

namespace goose {

class ARTBuilder {
public:
	ARTBuilder() = delete;
	ARTBuilder(ArenaAllocator &arena, ART &art, const unsafe_vector<ARTKey> &keys, const unsafe_vector<ARTKey> &row_ids)
	    : arena(arena), art(art), keys(keys), row_ids(row_ids) {
	}

public:
	//! Initialize the ART builder by passing a reference to the root node.
	void Init(Node &node, const idx_t end) {
		s.emplace(node, 0, end, 0);
	}
	//! Build the ART starting at the first entry in the stack.
	ARTConflictType Build();

private:
	struct NodeEntry {
		NodeEntry() = delete;
		NodeEntry(Node &node, const idx_t start, const idx_t end, const idx_t depth)
		    : node(node), start(start), end(end), depth(depth) {};

		Node &node;
		idx_t start;
		idx_t end;
		idx_t depth;
	};

	//! The arena holds any temporary memory allocated during the Build phase.
	ArenaAllocator &arena;
	//! The ART holding the node memory.
	ART &art;
	//! The keys to build the ART from.
	const unsafe_vector<ARTKey> &keys;
	//! The row IDs matching the keys.
	const unsafe_vector<ARTKey> &row_ids;
	//! The stack. While merging, NodeEntry elements are pushed onto of the stack.
	stack<NodeEntry> s;
};

} // namespace goose
