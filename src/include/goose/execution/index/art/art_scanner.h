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
