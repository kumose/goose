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

#include <goose/catalog/catalog_entry/function_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/function/function.h>
#include <goose/parser/parsed_data/create_aggregate_function_info.h>
#include <goose/main/attached_database.h>

namespace goose {

//! An aggregate function in the catalog
class AggregateFunctionCatalogEntry : public FunctionEntry {
public:
	static constexpr const CatalogType Type = CatalogType::AGGREGATE_FUNCTION_ENTRY;
	static constexpr const char *Name = "aggregate function";

public:
	AggregateFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateAggregateFunctionInfo &info)
	    : FunctionEntry(CatalogType::AGGREGATE_FUNCTION_ENTRY, catalog, schema, info), functions(info.functions) {
		for (auto &function : functions.functions) {
			function.catalog_name = catalog.GetAttached().GetName();
			function.schema_name = schema.name;
		}
	}

	//! The aggregate functions
	AggregateFunctionSet functions;
};
} // namespace goose
