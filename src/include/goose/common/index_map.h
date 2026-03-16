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

#include <goose/common/constants.h>
#include <goose/common/types-import.h>

namespace goose {

struct LogicalIndexHashFunction {
	uint64_t operator()(const LogicalIndex &index) const {
		return std::hash<idx_t>()(index.index);
	}
};

struct PhysicalIndexHashFunction {
	uint64_t operator()(const PhysicalIndex &index) const {
		return std::hash<idx_t>()(index.index);
	}
};

template <typename T>
using logical_index_map_t = unordered_map<LogicalIndex, T, LogicalIndexHashFunction>;

using logical_index_set_t = unordered_set<LogicalIndex, LogicalIndexHashFunction>;

template <typename T>
using physical_index_map_t = unordered_map<PhysicalIndex, T, PhysicalIndexHashFunction>;

using physical_index_set_t = unordered_set<PhysicalIndex, PhysicalIndexHashFunction>;

} // namespace goose
