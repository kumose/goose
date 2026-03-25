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
//


#pragma once

#include <goose/catalog/catalog_set.h>
#include <goose/catalog/dependency_manager.h>

namespace goose {
    //! This class mocks the CatalogSet interface, but does not actually store CatalogEntries
    class DependencyCatalogSet {
    public:
        DependencyCatalogSet(CatalogSet &set, const CatalogEntryInfo &info)
            : set(set), info(info), mangled_name(DependencyManager::MangleName(info)) {
        }

    public:
        bool CreateEntry(CatalogTransaction transaction, const MangledEntryName &name, unique_ptr<CatalogEntry> value);

        CatalogSet::EntryLookup GetEntryDetailed(CatalogTransaction transaction, const MangledEntryName &name);

        optional_ptr<CatalogEntry> GetEntry(CatalogTransaction transaction, const MangledEntryName &name);

        void Scan(CatalogTransaction transaction, const std::function<void(CatalogEntry &)> &callback);

        bool DropEntry(CatalogTransaction transaction, const MangledEntryName &name, bool cascade,
                       bool allow_drop_internal = false);

    private:
        MangledDependencyName ApplyPrefix(const MangledEntryName &name) const;

    public:
        CatalogSet &set;
        CatalogEntryInfo info;
        MangledEntryName mangled_name;
    };
} // namespace goose
