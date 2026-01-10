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

//! Node256 holds up to 256 children. They are indexed by their key byte.
class Node256 {
	friend class Node48;

public:
	static constexpr NType NODE_256 = NType::NODE_256;
	static constexpr uint16_t CAPACITY = 256;
	static constexpr uint8_t SHRINK_THRESHOLD = 36;

public:
	Node256() = delete;
	Node256(const Node256 &) = delete;
	Node256 &operator=(const Node256 &) = delete;

private:
	uint16_t count;
	Node children[CAPACITY];

public:
	//! Get a new Node256 handle and initialize the Node256.
	static NodeHandle<Node256> New(ART &art, Node &node) {
		node = Node::GetAllocator(art, NODE_256).New();
		node.SetMetadata(static_cast<uint8_t>(NODE_256));

		NodeHandle<Node256> handle(art, node);
		auto &n = handle.Get();

		// Reset the node (count and children).
		n.count = 0;
		for (uint16_t i = 0; i < CAPACITY; i++) {
			n.children[i].Clear();
		}

		return handle;
	}

	//! Insert a child at byte.
	static void InsertChild(ART &art, Node &node, const uint8_t byte, const Node child);
	//! Delete the child at byte.
	static void DeleteChild(ART &art, Node &node, const uint8_t byte);
	//! Replace the child at byte.
	void ReplaceChild(const uint8_t byte, const Node child) {
		D_ASSERT(count > SHRINK_THRESHOLD);
		auto status = children[byte].GetGateStatus();
		children[byte] = child;
		if (status == GateStatus::GATE_SET && child.HasMetadata()) {
			children[byte].SetGateStatus(status);
		}
	}

public:
	template <class F, class NODE>
	static void Iterator(NODE &n, F &&lambda) {
		D_ASSERT(n.count);
		for (idx_t i = 0; i < CAPACITY; i++) {
			if (n.children[i].HasMetadata()) {
				lambda(n.children[i]);
			}
		}
	}

	template <class NODE>
	static unsafe_optional_ptr<Node> GetChild(NODE &n, const uint8_t byte, const bool unsafe = false) {
		if (unsafe) {
			return &n.children[byte];
		}
		if (n.children[byte].HasMetadata()) {
			return &n.children[byte];
		}
		return nullptr;
	}

	template <class NODE>
	static unsafe_optional_ptr<Node> GetNextChild(NODE &n, uint8_t &byte) {
		for (idx_t i = byte; i < CAPACITY; i++) {
			if (n.children[i].HasMetadata()) {
				byte = UnsafeNumericCast<uint8_t>(i);
				return &n.children[i];
			}
		}
		return nullptr;
	}

	//! Extracts the bytes and their respective children.
	//! The return value is valid as long as the arena is valid.
	//! The node must be freed after calling into this function.
	NodeChildren ExtractChildren(ArenaAllocator &arena) {
		auto mem_bytes = arena.AllocateAligned(sizeof(uint8_t) * count);
		array_ptr<uint8_t> bytes(mem_bytes, count);
		auto mem_children = arena.AllocateAligned(sizeof(Node) * count);
		array_ptr<Node> children_ptr(reinterpret_cast<Node *>(mem_children), count);

		uint16_t ptr_idx = 0;
		for (idx_t i = 0; i < CAPACITY; i++) {
			if (children[i].HasMetadata()) {
				bytes[ptr_idx] = UnsafeNumericCast<uint8_t>(i);
				children_ptr[ptr_idx] = children[i];
				ptr_idx++;
			}
		}

		return NodeChildren(bytes, children_ptr);
	}

private:
	static void GrowNode48(ART &art, Node &node256, Node &node48);
};
} // namespace goose
