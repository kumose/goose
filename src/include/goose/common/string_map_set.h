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

#include <goose/common/operator/comparison_operators.h>
#include <goose/common/types/hash.h>
#include <goose/common/types/string_type.h>
#include <goose/common/types-import.h>

namespace goose {

struct StringHash {
	std::size_t operator()(const string_t &k) const {
		return Hash(k);
	}
};

struct StringEquality {
	bool operator()(const string_t &a, const string_t &b) const {
		return Equals::Operation(a, b);
	}
};

struct StringCIHash {
	std::size_t operator()(const string_t &k) const {
		return StringUtil::CIHash(k.GetData(), k.GetSize());
	}
};

struct StringCIEquality {
	bool operator()(const string_t &a, const string_t &b) const {
		return StringUtil::CIEquals(a.GetData(), a.GetSize(), b.GetData(), b.GetSize());
	}
};

template <typename T>
using string_map_t = unordered_map<string_t, T, StringHash, StringEquality>;

using string_set_t = unordered_set<string_t, StringHash, StringEquality>;

template <typename T>
using case_insensitive_string_map_t = unordered_map<string_t, T, StringCIHash, StringCIEquality>;

using case_insensitive_string_set_t = unordered_set<string_t, StringCIHash, StringCIEquality>;

} // namespace goose
