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

#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types/value.h>

namespace goose {

struct ValueHashFunction {
	uint64_t operator()(const Value &value) const {
		return (uint64_t)value.Hash();
	}
};

struct ValueEquality {
	bool operator()(const Value &a, const Value &b) const {
		return Value::NotDistinctFrom(a, b);
	}
};

template <typename T>
using value_map_t = unordered_map<Value, T, ValueHashFunction, ValueEquality>;

using value_set_t = unordered_set<Value, ValueHashFunction, ValueEquality>;

} // namespace goose
