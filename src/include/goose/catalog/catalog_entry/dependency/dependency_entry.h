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
#include <goose/common/enums/catalog_type.h>
#include <goose/common/exception.h>
#include <goose/common/types-import.h>
#include <goose/common/optional_ptr.h>
#include <goose/catalog/catalog_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/catalog/dependency.h>
#include <goose/catalog/dependency_manager.h>
#include <memory>

namespace goose {

class DependencyManager;

class DependencySetCatalogEntry;

//! Resembles a connection between an object and the CatalogEntry that can be retrieved from the Catalog using the
//! identifiers listed here

enum class DependencyEntryType : uint8_t { SUBJECT, DEPENDENT };

class DependencyEntry : public InCatalogEntry {
public:
	~DependencyEntry() override;

protected:
	DependencyEntry(Catalog &catalog, DependencyEntryType type, const MangledDependencyName &name,
	                const DependencyInfo &info);

public:
	const MangledEntryName &SubjectMangledName() const;
	const DependencySubject &Subject() const;

	const MangledEntryName &DependentMangledName() const;
	const DependencyDependent &Dependent() const;

	virtual const CatalogEntryInfo &EntryInfo() const = 0;
	virtual const MangledEntryName &EntryMangledName() const = 0;
	virtual const CatalogEntryInfo &SourceInfo() const = 0;
	virtual const MangledEntryName &SourceMangledName() const = 0;

public:
	DependencyEntryType Side() const;

protected:
	const MangledEntryName dependent_name;
	const MangledEntryName subject_name;
	const DependencyDependent dependent;
	const DependencySubject subject;

private:
	DependencyEntryType side;
};

} // namespace goose
