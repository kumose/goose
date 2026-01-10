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

#include <goose/common/types/hash.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/planner/column_binding.h>

namespace goose {

struct ColumnBindingHashFunction {
	uint64_t operator()(const ColumnBinding &a) const {
		return CombineHash(Hash<idx_t>(a.table_index), Hash<idx_t>(a.column_index));
	}
};

struct ColumnBindingEquality {
	bool operator()(const ColumnBinding &a, const ColumnBinding &b) const {
		return a == b;
	}
};

template <typename T>
using column_binding_map_t = unordered_map<ColumnBinding, T, ColumnBindingHashFunction, ColumnBindingEquality>;

using column_binding_set_t = unordered_set<ColumnBinding, ColumnBindingHashFunction, ColumnBindingEquality>;

} // namespace goose
