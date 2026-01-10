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
#include <goose/common/string.h>
#include <goose/common/string_util.h>
#include <goose/common/helper.h>

namespace goose {

struct CaseInsensitiveStringHashFunction {
	uint64_t operator()(const string &str) const {
		return StringUtil::CIHash(str);
	}
};

struct CaseInsensitiveStringEquality {
	bool operator()(const string &a, const string &b) const {
		return StringUtil::CIEquals(a, b);
	}
};

template <typename T>
using case_insensitive_map_t =
    unordered_map<string, T, CaseInsensitiveStringHashFunction, CaseInsensitiveStringEquality>;

using case_insensitive_set_t = unordered_set<string, CaseInsensitiveStringHashFunction, CaseInsensitiveStringEquality>;

struct CaseInsensitiveStringCompare {
	bool operator()(const string &s1, const string &s2) const {
		return StringUtil::CILessThan(s1, s2);
	}
};

template <typename T>
using case_insensitive_tree_t = map<string, T, CaseInsensitiveStringCompare>;

} // namespace goose
