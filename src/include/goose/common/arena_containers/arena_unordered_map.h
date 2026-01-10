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

#include <goose/common/arena_stl_allocator.h>
#include <goose/common/types-import.h>

namespace goose {

template <class KEY, class VALUE, class HASH = std::hash<KEY>, class PRED = std::equal_to<KEY>>
using arena_unordered_map = unordered_map<KEY, VALUE, HASH, PRED, arena_stl_allocator<pair<const KEY, VALUE>>>;

} // namespace goose
