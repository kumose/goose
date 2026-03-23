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

#include <goose/parser/qualified_name.h>
#include <goose/common/types-import.h>

namespace goose {

struct QualifiedColumnHashFunction {
	uint64_t operator()(const QualifiedColumnName &a) const {
		// hash only on the column name - since we match based on the shortest possible match
		return StringUtil::CIHash(a.column);
	}
};

struct QualifiedColumnEquality {
	bool operator()(const QualifiedColumnName &a, const QualifiedColumnName &b) const {
		// qualified column names follow a prefix comparison
		// so "tbl.i"  and "i" are equivalent, as are "schema.tbl.i" and "i"
		// but "tbl.i" and "tbl2.i" are not equivalent
		if (!a.catalog.empty() && !b.catalog.empty() && !StringUtil::CIEquals(a.catalog, b.catalog)) {
			return false;
		}
		if (!a.schema.empty() && !b.schema.empty() && !StringUtil::CIEquals(a.schema, b.schema)) {
			return false;
		}
		if (!a.table.empty() && !b.table.empty() && !StringUtil::CIEquals(a.table, b.table)) {
			return false;
		}
		return StringUtil::CIEquals(a.column, b.column);
	}
};

using qualified_column_set_t = unordered_set<QualifiedColumnName, QualifiedColumnHashFunction, QualifiedColumnEquality>;

template <class T>
using qualified_column_map_t =
    unordered_map<QualifiedColumnName, T, QualifiedColumnHashFunction, QualifiedColumnEquality>;

} // namespace goose
