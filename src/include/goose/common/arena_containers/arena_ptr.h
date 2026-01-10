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

namespace goose {

//! Call destructor without attempting to free the memory
template <class T>
struct arena_deleter { // NOLINT: match stl case
	void operator()(T *pointer) {
		pointer->~T();
	}
};

template <class T>
using arena_ptr = unique_ptr<T, arena_deleter<T>>;

template <class T>
using unsafe_arena_ptr = unique_ptr<T, arena_deleter<T>, false>;

} // namespace goose
