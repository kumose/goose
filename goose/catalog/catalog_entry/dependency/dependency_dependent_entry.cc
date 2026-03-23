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

#include <goose/catalog/catalog_entry/dependency/dependency_dependent_entry.h>

namespace goose {
    DependencyDependentEntry::DependencyDependentEntry(Catalog &catalog, const DependencyInfo &info)
        : DependencyEntry(catalog, DependencyEntryType::DEPENDENT,
                          MangledDependencyName(DependencyManager::MangleName(info.subject.entry),
                                                DependencyManager::MangleName(info.dependent.entry)),
                          info) {
    }

    const MangledEntryName &DependencyDependentEntry::EntryMangledName() const {
        return dependent_name;
    }

    const CatalogEntryInfo &DependencyDependentEntry::EntryInfo() const {
        return dependent.entry;
    }

    const MangledEntryName &DependencyDependentEntry::SourceMangledName() const {
        return subject_name;
    }

    const CatalogEntryInfo &DependencyDependentEntry::SourceInfo() const {
        return subject.entry;
    }

    DependencyDependentEntry::~DependencyDependentEntry() {
    }
} // namespace goose
