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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/main/profiling_node.h>
#include <goose/common/tree_renderer.h>
#include <goose/common/render_tree.h>

namespace goose {
class LogicalOperator;
class PhysicalOperator;
class Pipeline;
struct PipelineRenderNode;

class HTMLTreeRenderer : public TreeRenderer {
public:
	explicit HTMLTreeRenderer() {
	}
	~HTMLTreeRenderer() override {
	}

public:
	string ToString(const LogicalOperator &op);
	string ToString(const PhysicalOperator &op);
	string ToString(const ProfilingNode &op);
	string ToString(const Pipeline &op);

	void Render(const LogicalOperator &op, std::ostream &ss);
	void Render(const PhysicalOperator &op, std::ostream &ss);
	void Render(const ProfilingNode &op, std::ostream &ss) override;
	void Render(const Pipeline &op, std::ostream &ss);

	void ToStreamInternal(RenderTree &root, std::ostream &ss) override;
};

} // namespace goose
