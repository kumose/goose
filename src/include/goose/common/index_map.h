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
