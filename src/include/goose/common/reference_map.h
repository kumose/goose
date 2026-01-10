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
#include <goose/common/types-import.h>

namespace goose {
class Expression;

template <class T>
struct ReferenceHashFunction {
	uint64_t operator()(const reference<T> &ref) const {
		return std::hash<void *>()((void *)&ref.get());
	}
};

template <class T>
struct ReferenceEquality {
	bool operator()(const reference<T> &a, const reference<T> &b) const {
		return &a.get() == &b.get();
	}
};

template <typename T, typename TGT>
using reference_map_t = unordered_map<reference<T>, TGT, ReferenceHashFunction<T>, ReferenceEquality<T>>;

template <typename T>
using reference_set_t = unordered_set<reference<T>, ReferenceHashFunction<T>, ReferenceEquality<T>>;

} // namespace goose
