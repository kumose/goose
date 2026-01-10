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

#include <goose/catalog/default/default_generator.h>
#include <goose/parser/parsed_data/create_macro_info.h>

namespace goose {
class SchemaCatalogEntry;

struct DefaultNamedParameter {
	const char *name;
	const char *default_value;
};

struct DefaultTableMacro {
	const char *schema;
	const char *name;
	const char *parameters[8];
	DefaultNamedParameter named_parameters[8];
	const char *macro;
};

class DefaultTableFunctionGenerator : public DefaultGenerator {
public:
	DefaultTableFunctionGenerator(Catalog &catalog, SchemaCatalogEntry &schema);

	SchemaCatalogEntry &schema;

public:
	unique_ptr<CatalogEntry> CreateDefaultEntry(ClientContext &context, const string &entry_name) override;
	vector<string> GetDefaultEntries() override;

	static unique_ptr<CreateMacroInfo> CreateTableMacroInfo(const DefaultTableMacro &default_macro);

private:
	static unique_ptr<CreateMacroInfo> CreateInternalTableMacroInfo(const DefaultTableMacro &default_macro,
	                                                                unique_ptr<MacroFunction> function);
};

} // namespace goose
