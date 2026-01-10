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
