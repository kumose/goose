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
