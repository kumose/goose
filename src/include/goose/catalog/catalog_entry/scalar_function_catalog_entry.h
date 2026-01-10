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
#include <goose/parser/parsed_data/create_scalar_function_info.h>

namespace goose {

//! A scalar function in the catalog
class ScalarFunctionCatalogEntry : public FunctionEntry {
public:
	static constexpr const CatalogType Type = CatalogType::SCALAR_FUNCTION_ENTRY;
	static constexpr const char *Name = "scalar function";

public:
	ScalarFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateScalarFunctionInfo &info);

	//! The scalar functions
	ScalarFunctionSet functions;

public:
	unique_ptr<CatalogEntry> AlterEntry(CatalogTransaction transaction, AlterInfo &info) override;
};
} // namespace goose
