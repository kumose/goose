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

#include <goose/common/unique_ptr.h>
#include <goose/common/string.h>
#include <goose/common/optional_ptr.h>
#include <goose/main/query_profiler.h>
#include <goose/main/profiling_node.h>

namespace goose {
class LogicalOperator;
class PhysicalOperator;
class Pipeline;
struct PipelineRenderNode;

struct RenderTreeNode {
public:
	static constexpr const char *CARDINALITY = "__cardinality__";
	static constexpr const char *ESTIMATED_CARDINALITY = "__estimated_cardinality__";
	static constexpr const char *TIMING = "__timing__";

public:
	struct Coordinate {
	public:
		Coordinate(idx_t x, idx_t y) : x(x), y(y) {
		}

	public:
		idx_t x;
		idx_t y;
	};
	RenderTreeNode(const string &name, InsertionOrderPreservingMap<string> extra_text)
	    : name(name), extra_text(std::move(extra_text)) {
	}

public:
	void AddChildPosition(idx_t x, idx_t y) {
		child_positions.emplace_back(x, y);
	}

public:
	string name;
	InsertionOrderPreservingMap<string> extra_text;
	vector<Coordinate> child_positions;
};

struct RenderTree {
	RenderTree(idx_t width, idx_t height);

	unique_array<unique_ptr<RenderTreeNode>> nodes;
	idx_t width;
	idx_t height;

public:
	static unique_ptr<RenderTree> CreateRenderTree(const LogicalOperator &op);
	static unique_ptr<RenderTree> CreateRenderTree(const PhysicalOperator &op);
	static unique_ptr<RenderTree> CreateRenderTree(const ProfilingNode &op);
	static unique_ptr<RenderTree> CreateRenderTree(const Pipeline &op);

public:
	optional_ptr<RenderTreeNode> GetNode(idx_t x, idx_t y);
	void SetNode(idx_t x, idx_t y, unique_ptr<RenderTreeNode> node);
	bool HasNode(idx_t x, idx_t y);
	void SanitizeKeyNames();

private:
	idx_t GetPosition(idx_t x, idx_t y);
};

} // namespace goose
