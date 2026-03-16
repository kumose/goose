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
