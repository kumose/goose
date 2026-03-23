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

#include <goose/common/common.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>

namespace goose {
    class CatalogEntry;

    struct CatalogEntryHashFunction {
        uint64_t operator()(const reference<CatalogEntry> &a) const {
            std::hash<void *> hash_func;
            return hash_func((void *) &a.get());
        }
    };

    struct CatalogEntryEquality {
        bool operator()(const reference<CatalogEntry> &a, const reference<CatalogEntry> &b) const {
            return RefersToSameObject(a, b);
        }
    };

    using catalog_entry_set_t = unordered_set<reference<CatalogEntry>, CatalogEntryHashFunction, CatalogEntryEquality>;

    template<typename T>
    using catalog_entry_map_t = unordered_map<reference<CatalogEntry>, T, CatalogEntryHashFunction,
        CatalogEntryEquality>;

    using catalog_entry_vector_t = vector<reference<CatalogEntry> >;
} // namespace goose
