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

#include <goose/common/types-import.h>
#include <goose/execution/index/art/prefix.h>
#include <goose/execution/index/art/base_node.h>
#include <goose/execution/index/art/node48.h>
#include <goose/execution/index/art/node256.h>

namespace goose {

enum class ARTScanHandling : uint8_t {
	EMPLACE,
	POP,
};

//! ARTScanner scans the entire ART and processes each node.
template <ARTScanHandling HANDLING, class NODE>
class ARTScanner {
public:
	template <class FUNC>
	explicit ARTScanner(ART &art, FUNC &&handler, NODE &root) : art(art) {
		Emplace(handler, root);
	}

public:
	template <class FUNC>
	void Scan(FUNC &&handler) {
		while (!s.empty()) {
			auto &entry = s.top();
			if (entry.exhausted) {
				Pop(handler, entry.node);
				continue;
			}
			entry.exhausted = true;

			const auto type = entry.node.GetType();
			switch (type) {
			case NType::LEAF_INLINED:
			case NType::LEAF:
			case NType::NODE_7_LEAF:
			case NType::NODE_15_LEAF:
			case NType::NODE_256_LEAF:
				break;
			case NType::PREFIX: {
				Prefix prefix(art, entry.node, true);
				Emplace(handler, *prefix.ptr);
				break;
			}

			case NType::NODE_4: {
				IterateChildren<FUNC, Node4>(handler, entry.node, type);
				break;
			}
			case NType::NODE_16: {
				IterateChildren<FUNC, Node16>(handler, entry.node, type);
				break;
			}
			case NType::NODE_48: {
				IterateChildren<FUNC, Node48>(handler, entry.node, type);
				break;
			}
			case NType::NODE_256: {
				IterateChildren<FUNC, Node256>(handler, entry.node, type);
				break;
			}
			default:
				throw InternalException("invalid node type for ART ARTScanner: %d", type);
			}
		}
	}

private:
	template <class FUNC>
	void Emplace(FUNC &&handler, NODE &node) {
		if (HANDLING == ARTScanHandling::EMPLACE) {
			auto result = handler(node);
			if (result == ARTHandlingResult::SKIP) {
				return;
			}
			D_ASSERT(result == ARTHandlingResult::CONTINUE);
		}
		s.emplace(node);
	}

	template <class FUNC>
	void Pop(FUNC &&handler, NODE &node) {
		if (HANDLING == ARTScanHandling::POP) {
			handler(node);
		}
		s.pop();
	}

	template <class FUNC, class NODE_TYPE>
	void IterateChildren(FUNC &&handler, NODE &node, const NType type) {
		auto &n = Node::Ref<NODE_TYPE>(art, node, type);
		NODE_TYPE::Iterator(n, [&](NODE &child) { Emplace(handler, child); });
	}

private:
	struct NodeEntry {
		NodeEntry() = delete;
		explicit NodeEntry(NODE &node) : node(node), exhausted(false) {};

		NODE &node;
		bool exhausted;
	};

	ART &art;
	stack<NodeEntry> s;
};

} // namespace goose
