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

#include <goose/catalog/catalog_set.h>
#include <goose/catalog/catalog_entry/function_entry.h>
#include <goose/function/macro_function.h>
#include <goose/parser/parsed_data/create_macro_info.h>
#include <goose/function/function_set.h>

namespace goose {

//! A macro function in the catalog
class MacroCatalogEntry : public FunctionEntry {
public:
	MacroCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateMacroInfo &info);

	//! The macro function
	vector<unique_ptr<MacroFunction>> macros;

public:
	unique_ptr<CreateInfo> GetInfo() const override;

	string ToSQL() const override;
};

} // namespace goose
