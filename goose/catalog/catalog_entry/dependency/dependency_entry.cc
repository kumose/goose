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

#include <goose/catalog/catalog_entry/dependency/dependency_entry.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/dependency_manager.h>
#include <goose/catalog/catalog.h>

namespace goose {

DependencyEntry::DependencyEntry(Catalog &catalog, DependencyEntryType side, const MangledDependencyName &name,
                                 const DependencyInfo &info)
    : InCatalogEntry(CatalogType::DEPENDENCY_ENTRY, catalog, name.name),
      dependent_name(DependencyManager::MangleName(info.dependent.entry)),
      subject_name(DependencyManager::MangleName(info.subject.entry)), dependent(info.dependent), subject(info.subject),
      side(side) {
	D_ASSERT(info.dependent.entry.type != CatalogType::DEPENDENCY_ENTRY);
	D_ASSERT(info.subject.entry.type != CatalogType::DEPENDENCY_ENTRY);
	if (catalog.IsTemporaryCatalog()) {
		temporary = true;
	}
}

const MangledEntryName &DependencyEntry::SubjectMangledName() const {
	return subject_name;
}

const DependencySubject &DependencyEntry::Subject() const {
	return subject;
}

const MangledEntryName &DependencyEntry::DependentMangledName() const {
	return dependent_name;
}

const DependencyDependent &DependencyEntry::Dependent() const {
	return dependent;
}

DependencyEntry::~DependencyEntry() {
}

DependencyEntryType DependencyEntry::Side() const {
	return side;
}

} // namespace goose
