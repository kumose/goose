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

#include <goose/catalog/catalog_entry.h>
#include <goose/catalog/dependency_list.h>

namespace goose {
class SchemaCatalogEntry;

//! A StandardEntry is a catalog entry that is a member of a schema
class StandardEntry : public InCatalogEntry {
public:
	StandardEntry(CatalogType type, SchemaCatalogEntry &schema, Catalog &catalog, string name)
	    : InCatalogEntry(type, catalog, std::move(name)), schema(schema) {
	}
	~StandardEntry() override {
	}

	//! The schema the entry belongs to
	SchemaCatalogEntry &schema;
	//! The dependencies of the entry, can be empty
	LogicalDependencyList dependencies;

public:
	SchemaCatalogEntry &ParentSchema() override {
		return schema;
	}
	const SchemaCatalogEntry &ParentSchema() const override {
		return schema;
	}
};

} // namespace goose
