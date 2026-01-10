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

#include <goose/catalog/standard_entry.h>
#include <goose/common/types-import.h>
#include <goose/function/copy_function.h>

namespace goose {

class Catalog;
struct CreateCopyFunctionInfo;

//! A table function in the catalog
class CopyFunctionCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::COPY_FUNCTION_ENTRY;
	static constexpr const char *Name = "copy function";

public:
	CopyFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateCopyFunctionInfo &info);

	//! The copy function
	CopyFunction function;
};
} // namespace goose
