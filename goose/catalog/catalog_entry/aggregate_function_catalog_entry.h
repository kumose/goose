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
