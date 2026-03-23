// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
