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
