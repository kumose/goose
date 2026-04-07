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
