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
#include <goose/common/types/hash.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>

namespace goose {

struct LogicalTypeHashFunction {
	uint64_t operator()(const LogicalType &type) const {
		return (uint64_t)type.Hash();
	}
};

struct LogicalTypeEquality {
	bool operator()(const LogicalType &a, const LogicalType &b) const {
		return a == b;
	}
};

template <typename T>
using type_map_t = unordered_map<LogicalType, T, LogicalTypeHashFunction, LogicalTypeEquality>;

using type_set_t = unordered_set<LogicalType, LogicalTypeHashFunction, LogicalTypeEquality>;

struct LogicalTypeIdHashFunction {
	uint64_t operator()(const LogicalTypeId &type_id) const {
		return goose::Hash<uint8_t>((uint8_t)type_id);
	}
};

struct LogicalTypeIdEquality {
	bool operator()(const LogicalTypeId &a, const LogicalTypeId &b) const {
		return a == b;
	}
};

template <typename T>
using type_id_map_t = unordered_map<LogicalTypeId, T, LogicalTypeIdHashFunction, LogicalTypeIdEquality>;

using type_id_set_t = unordered_set<LogicalTypeId, LogicalTypeIdHashFunction, LogicalTypeIdEquality>;

} // namespace goose
