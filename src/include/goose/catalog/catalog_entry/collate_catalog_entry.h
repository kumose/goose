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
#include <goose/function/function.h>
#include <goose/parser/parsed_data/create_collation_info.h>

namespace goose {

//! A collation catalog entry
class CollateCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::COLLATION_ENTRY;
	static constexpr const char *Name = "collation";

public:
	CollateCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateCollationInfo &info)
	    : StandardEntry(CatalogType::COLLATION_ENTRY, schema, catalog, info.name), function(info.function),
	      combinable(info.combinable), not_required_for_equality(info.not_required_for_equality) {
	}

	//! The collation function to push in case collation is required
	ScalarFunction function;
	//! Whether or not the collation can be combined with other collations.
	bool combinable;
	//! Whether or not the collation is required for equality comparisons or not. For many collations a binary
	//! comparison for equality comparisons is correct, allowing us to skip the collation in these cases which greatly
	//! speeds up processing.
	bool not_required_for_equality;
};
} // namespace goose
