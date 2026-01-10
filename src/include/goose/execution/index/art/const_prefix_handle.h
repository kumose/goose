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

//! ConstPrefixHandle is a read-only wrapper to access a prefix node.
//! The prefix contains up to the ART's prefix size bytes, an additional byte for the count,
//! and a Node pointer to a child node.
//! A segment handle is used for memory management, but it is not marked as modified.
class ConstPrefixHandle {
public:
	static constexpr NType PREFIX = NType::PREFIX;

	ConstPrefixHandle(const ART &art, const Node node);
	ConstPrefixHandle() = delete;
	ConstPrefixHandle(const ConstPrefixHandle &) = delete;
	ConstPrefixHandle &operator=(const ConstPrefixHandle &) = delete;
	ConstPrefixHandle(ConstPrefixHandle &&) = delete;
	ConstPrefixHandle &operator=(ConstPrefixHandle &&) = delete;

public:
	uint8_t GetCount(const ART &art) const;
	uint8_t GetByte(const idx_t pos) const;

	//! Traverses and verifies the node and its subtree.
	static void Verify(ART &art, const Node &node);

	//! Returns the string representation of the node using ToStringOptions.
	static string ToString(ART &art, const Node &node, const ToStringOptions &options);

private:
	template <class F>
	static void Iterator(ART &art, reference<const Node> &ref, const bool exit_gate, F &&lambda) {
		while (ref.get().HasMetadata() && ref.get().GetType() == PREFIX) {
			ConstPrefixHandle handle(art, ref);
			lambda(handle);

			ref = *handle.child;
			if (exit_gate && ref.get().GetGateStatus() == GateStatus::GATE_SET) {
				break;
			}
		}
	}

private:
	SegmentHandle segment_handle;
	data_ptr_t data;
	Node *child;
};

} // namespace goose
