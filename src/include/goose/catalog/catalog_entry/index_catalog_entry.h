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
#include <goose/parser/parsed_data/create_index_info.h>

namespace goose {

struct DataTableInfo;

//! An index catalog entry
class IndexCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::INDEX_ENTRY;
	static constexpr const char *Name = "index";

public:
	//! Create an IndexCatalogEntry
	IndexCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateIndexInfo &info);

	//! The SQL of the CREATE INDEX statement
	string sql;
	//! Additional index options
	case_insensitive_map_t<Value> options;

	//! The index type (ART, B+-tree, Skip-List, ...)
	string index_type;
	//! The index constraint type
	IndexConstraintType index_constraint_type;
	//! The column ids of the indexed table
	vector<column_t> column_ids;
	//! The set of expressions to index by
	vector<unique_ptr<ParsedExpression>> expressions;
	vector<unique_ptr<ParsedExpression>> parsed_expressions;

public:
	//! Returns the CreateIndexInfo
	unique_ptr<CreateInfo> GetInfo() const override;
	//! Returns the original CREATE INDEX SQL
	string ToSQL() const override;

	virtual string GetSchemaName() const = 0;
	virtual string GetTableName() const = 0;

	//! Returns true, if this index is UNIQUE
	bool IsUnique() const;
	//! Returns true, if this index is a PRIMARY KEY
	bool IsPrimary() const;
};

} // namespace goose
