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

#include <goose/common/common.h>
#include <goose/common/deque.h>
#include <goose/common/enums/profiler_format.h>
#include <goose/common/types-import.h>
#include <goose/common/profiler.h>
#include <goose/common/reference_map.h>
#include <goose/common/string_util.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/common/winapi.h>
#include <goose/execution/expression_executor_state.h>
#include <goose/execution/physical_operator.h>
#include <goose/main/profiling_info.h>

namespace goose {

//! Recursive tree mirroring the operator tree.
class ProfilingNode {
public:
	explicit ProfilingNode() {
	}
	virtual ~ProfilingNode() {};

private:
	ProfilingInfo profiling_info;

public:
	idx_t depth = 0;
	vector<unique_ptr<ProfilingNode>> children;

public:
	idx_t GetChildCount() {
		return children.size();
	}
	ProfilingInfo &GetProfilingInfo() {
		return profiling_info;
	}
	const ProfilingInfo &GetProfilingInfo() const {
		return profiling_info;
	}
	optional_ptr<ProfilingNode> GetChild(idx_t idx) {
		return children[idx].get();
	}
	optional_ptr<ProfilingNode> AddChild(unique_ptr<ProfilingNode> child) {
		children.push_back(std::move(child));
		return children.back().get();
	}
};

} // namespace goose
