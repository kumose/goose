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

#include <goose/execution/index/fixed_size_allocator.h>
#include <goose/execution/index/art/art.h>
#include <goose/execution/index/art/node.h>

namespace goose {

//! There are three types of leaves.
//! 1. LEAF_INLINED: Inlines a row ID in a Node pointer.
//! 2. LEAF: Deprecated. A list of Leaf nodes containing row IDs.
//! 3. Nested leaves indicated by gate nodes. If an ART key contains multiple row IDs,
//! then we use the row IDs as keys and create a nested ART behind the gate node.
//! As row IDs are always unique, these nested ARTs never contain duplicates themselves.
class Leaf {
public:
	static constexpr NType LEAF = NType::LEAF;
	static constexpr NType INLINED = NType::LEAF_INLINED;

	static constexpr uint8_t LEAF_SIZE = 4; // Deprecated.

public:
	Leaf() = delete;
	Leaf(const Leaf &) = delete;
	Leaf &operator=(const Leaf &) = delete;

private:
	uint8_t count;            // Deprecated.
	row_t row_ids[LEAF_SIZE]; // Deprecated.
	Node ptr;                 // Deprecated.

public:
	//! Inline a row ID into a node pointer.
	static void New(Node &node, const row_t row_id);

	//! Merge two inlined leaf nodes.
	static void MergeInlined(ArenaAllocator &arena, ART &art, Node &left, Node &right, GateStatus status, idx_t depth);

	//! Transforms a deprecated leaf to a nested leaf.
	static void TransformToNested(ART &art, Node &node);
	//! Transforms a nested leaf to a deprecated leaf.
	static void TransformToDeprecated(ART &art, Node &node);

public:
	//! Frees the linked list of leaves.
	static void DeprecatedFree(ART &art, Node &node);
	//! Fills the row_ids vector with the row IDs of this linked list of leaves.
	//! Never pushes more than max_count row IDs.
	static bool DeprecatedGetRowIds(ART &art, const Node &node, set<row_t> &row_ids, const idx_t max_count);
	//! Vacuums the linked list of leaves.
	static void DeprecatedVacuum(ART &art, Node &node);

	//! Traverses and verifies the linked list of leaves.
	static void DeprecatedVerify(ART &art, const Node &node);
	//! Count the number of leaves.
	void DeprecatedVerifyAllocations(ART &art, unordered_map<uint8_t, idx_t> &node_counts) const;

	//! Return string representation of the linked list of leaves.
	//! If print_deprecated_leaves is false, returns "[deprecated leaves]" with proper indentation.
	static string DeprecatedToString(ART &art, const Node &node, const ToStringOptions &options);
};

} // namespace goose
