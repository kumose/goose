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
#include <goose/execution/index/art/node.h>
#include <goose/execution/index/art/node256.h>
#include <goose/execution/index/fixed_size_allocator.h>

namespace goose {

//! Node256Leaf is a bitmask containing 256 bits.
class Node256Leaf {
	friend class Node15Leaf;

public:
	static constexpr NType NODE_256_LEAF = NType::NODE_256_LEAF;
	static constexpr uint16_t CAPACITY = Node256::CAPACITY;

public:
	Node256Leaf() = delete;
	Node256Leaf(const Node256Leaf &) = delete;
	Node256Leaf &operator=(const Node256Leaf &) = delete;

private:
	uint16_t count;
	validity_t mask[CAPACITY / sizeof(validity_t)];

public:
	//! Get a new Node256Leaf handle and initialize the leaf.
	static NodeHandle<Node256Leaf> New(ART &art, Node &node);

	//! Insert a byte.
	static void InsertByte(ART &art, Node &node, const uint8_t byte);
	//! Delete a byte.
	static void DeleteByte(ART &art, Node &node, const uint8_t byte);

	//! Returns true, if the byte exists, else false.
	bool HasByte(const uint8_t byte);

	//! Returns a pointer to the bytes in the leaf.
	//! The pointer data is valid as long as the arena is valid.
	array_ptr<uint8_t> GetBytes(ArenaAllocator &arena);

	//! Get the first byte greater or equal to the byte.
	//! Returns true, if such a byte exists, else false.
	bool GetNextByte(uint8_t &byte);

private:
	static void GrowNode15Leaf(ART &art, Node &node256_leaf, Node &node15_leaf);
};

} // namespace goose
