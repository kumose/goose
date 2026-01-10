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

#include <goose/common/types.h>
#include <goose/common/types-import.h>

namespace goose {

struct PerfectHash {
	std::size_t operator()(const idx_t &h) const {
		return h;
	}
};

struct PerfectEquality {
	bool operator()(const idx_t &a, const idx_t &b) const {
		return a == b;
	}
};

template <typename T>
using perfect_map_t = unordered_map<idx_t, T, PerfectHash, PerfectEquality>;

using perfect_set_t = unordered_set<idx_t, PerfectHash, PerfectEquality>;

} // namespace goose
