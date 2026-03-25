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
