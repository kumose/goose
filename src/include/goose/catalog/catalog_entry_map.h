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
#include <goose/common/types-import.h>

namespace goose {
class CatalogEntry;

struct CatalogEntryHashFunction {
	uint64_t operator()(const reference<CatalogEntry> &a) const {
		std::hash<void *> hash_func;
		return hash_func((void *)&a.get());
	}
};

struct CatalogEntryEquality {
	bool operator()(const reference<CatalogEntry> &a, const reference<CatalogEntry> &b) const {
		return RefersToSameObject(a, b);
	}
};

using catalog_entry_set_t = unordered_set<reference<CatalogEntry>, CatalogEntryHashFunction, CatalogEntryEquality>;

template <typename T>
using catalog_entry_map_t = unordered_map<reference<CatalogEntry>, T, CatalogEntryHashFunction, CatalogEntryEquality>;

using catalog_entry_vector_t = vector<reference<CatalogEntry>>;

} // namespace goose
