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

//! PrefixHandle is a mutable wrapper to access and modify a prefix node.
//! The prefix contains up to the ART's prefix size bytes, an additional byte for the count,
//! and a Node pointer to a child node.
//! PrefixHandle uses SegmentHandle for memory management and marks memory as modified.
//! For read-only access, use ConstPrefixHandle instead.
class PrefixHandle {
public:
	static constexpr NType PREFIX = NType::PREFIX;

	static constexpr uint8_t DEPRECATED_COUNT = 15;

public:
	PrefixHandle() = delete;
	PrefixHandle(const ART &art, const Node node);
	PrefixHandle(FixedSizeAllocator &allocator, const Node node, const uint8_t count);
	PrefixHandle(const PrefixHandle &) = delete;
	PrefixHandle(PrefixHandle &&other) noexcept;
	PrefixHandle &operator=(PrefixHandle &&other) noexcept;

	data_ptr_t data;
	Node *child;

public:
	//! Create a new deprecated prefix node and return a handle to it.
	static PrefixHandle NewDeprecated(FixedSizeAllocator &allocator, Node &node);

	static void TransformToDeprecated(ART &art, Node &node, TransformToDeprecatedState &state);

private:
	PrefixHandle TransformToDeprecatedAppend(ART &art, FixedSizeAllocator &allocator, const uint8_t byte);

private:
	SegmentHandle segment_handle;
};

} // namespace goose
