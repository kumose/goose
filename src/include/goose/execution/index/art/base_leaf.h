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

template <uint8_t CAPACITY, NType TYPE>
class BaseLeaf {
	friend class Node7Leaf;
	friend class Node15Leaf;
	friend class Node256Leaf;

public:
	BaseLeaf() = delete;
	BaseLeaf(const BaseLeaf &) = delete;
	BaseLeaf &operator=(const BaseLeaf &) = delete;

private:
	uint8_t count;
	uint8_t key[CAPACITY];

public:
	//! Get a new BaseLeaf and initialize it.
	static NodeHandle<BaseLeaf> New(ART &art, Node &node) {
		node = Node::GetAllocator(art, TYPE).New();
		node.SetMetadata(static_cast<uint8_t>(TYPE));

		NodeHandle<BaseLeaf> handle(art, node);
		auto &n = handle.Get();

		n.count = 0;
		return handle;
	}

	//! Returns true, if the byte exists, else false.
	bool HasByte(const uint8_t byte) const {
		for (uint8_t i = 0; i < count; i++) {
			if (key[i] == byte) {
				return true;
			}
		}
		return false;
	}

	//! Returns a pointer to the bytes in the leaf.
	//! The pointer's data is valid as long as the leaf is valid.
	array_ptr<uint8_t> GetBytes() {
		return array_ptr<uint8_t>(key, count);
	}

	//! Get the first byte greater than or equal to the byte.
	//! Returns true, if such a byte exists, else false.
	bool GetNextByte(uint8_t &byte) const {
		for (uint8_t i = 0; i < count; i++) {
			if (key[i] >= byte) {
				byte = key[i];
				return true;
			}
		}
		return false;
	}

private:
	static void InsertByteInternal(BaseLeaf &n, const uint8_t byte);
	static NodeHandle<BaseLeaf> DeleteByteInternal(ART &art, Node &node, const uint8_t byte);
};

//! Node7Leaf holds up to seven sorted bytes.
class Node7Leaf : public BaseLeaf<7, NType::NODE_7_LEAF> {
	friend class Node15Leaf;

public:
	static constexpr NType NODE_7_LEAF = NType::NODE_7_LEAF;
	static constexpr uint8_t CAPACITY = 7;
	static constexpr idx_t AND_LAST_BYTE = 0xFFFFFFFFFFFFFF00;

public:
	//! Insert a byte.
	static void InsertByte(ART &art, Node &node, const uint8_t byte);
	//! Delete a byte.
	static void DeleteByte(ART &art, Node &node, Node &prefix, const uint8_t byte, const ARTKey &row_id);

private:
	static void ShrinkNode15Leaf(ART &art, Node &node7_leaf, Node &node15_leaf);
};

//! Node15Leaf holds up to 15 sorted bytes.
class Node15Leaf : public BaseLeaf<15, NType::NODE_15_LEAF> {
	friend class Node7Leaf;
	friend class Node256Leaf;

public:
	static constexpr NType NODE_15_LEAF = NType::NODE_15_LEAF;
	static constexpr uint8_t CAPACITY = 15;

public:
	//! Insert a byte.
	static void InsertByte(ART &art, Node &node, const uint8_t byte);
	//! Delete a byte.
	static void DeleteByte(ART &art, Node &node, const uint8_t byte);

private:
	static void GrowNode7Leaf(ART &art, Node &node15_leaf, Node &node7_leaf);
	//! We shrink at <= Node48::SHRINK_THRESHOLD.
	static void ShrinkNode256Leaf(ART &art, Node &node15_leaf, Node &node256_leaf);
};

} // namespace goose
