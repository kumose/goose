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
#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/catalog/dependency_list.h>

namespace goose {

//! A type catalog entry
class TypeCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::TYPE_ENTRY;
	static constexpr const char *Name = "type";

public:
	//! Create a TypeCatalogEntry and initialize storage for it
	TypeCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateTypeInfo &info);

	LogicalType user_type;

	bind_logical_type_function_t bind_function;

public:
	unique_ptr<CreateInfo> GetInfo() const override;
	unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;

	string ToSQL() const override;
};
} // namespace goose
