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


#include <goose/catalog/dependency_catalog_set.h>
#include <goose/catalog/catalog_entry/dependency/dependency_entry.h>
#include <goose/catalog/dependency_list.h>

namespace goose {
    MangledDependencyName DependencyCatalogSet::ApplyPrefix(const MangledEntryName &name) const {
        return MangledDependencyName(mangled_name, name);
    }

    bool DependencyCatalogSet::CreateEntry(CatalogTransaction transaction, const MangledEntryName &name,
                                           unique_ptr<CatalogEntry> value) {
        auto new_name = ApplyPrefix(name);
        const LogicalDependencyList EMPTY_DEPENDENCIES;
        return set.CreateEntry(transaction, new_name.name, std::move(value), EMPTY_DEPENDENCIES);
    }

    CatalogSet::EntryLookup DependencyCatalogSet::GetEntryDetailed(CatalogTransaction transaction,
                                                                   const MangledEntryName &name) {
        auto new_name = ApplyPrefix(name);
        return set.GetEntryDetailed(transaction, new_name.name);
    }

    optional_ptr<CatalogEntry> DependencyCatalogSet::GetEntry(CatalogTransaction transaction,
                                                              const MangledEntryName &name) {
        auto new_name = ApplyPrefix(name);
        return set.GetEntry(transaction, new_name.name);
    }

    void DependencyCatalogSet::Scan(CatalogTransaction transaction,
                                    const std::function<void(CatalogEntry &)> &callback) {
        set.ScanWithPrefix(
            transaction,
            [&](CatalogEntry &entry) {
                auto &dep = entry.Cast<DependencyEntry>();
                auto &from = dep.SourceMangledName();
                if (!StringUtil::CIEquals(from.name, mangled_name.name)) {
                    return;
                }
                callback(entry);
            },
            mangled_name.name);
    }

    bool DependencyCatalogSet::DropEntry(CatalogTransaction transaction, const MangledEntryName &name, bool cascade,
                                         bool allow_drop_internal) {
        auto new_name = ApplyPrefix(name);
        return set.DropEntry(transaction, new_name.name, cascade, allow_drop_internal);
    }
} // namespace goose
