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

#include <goose/main/table_description.h>
#include <goose/parser/tableref.h>
#include <goose/parser/tableref/at_clause.h>

namespace goose {

//! Represents a TableReference to a base table in a catalog and schema.
class BaseTableRef : public TableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::BASE_TABLE;

public:
	BaseTableRef()
	    : TableRef(TableReferenceType::BASE_TABLE), catalog_name(INVALID_CATALOG), schema_name(INVALID_SCHEMA) {
	}
	explicit BaseTableRef(const TableDescription &description)
	    : TableRef(TableReferenceType::BASE_TABLE), catalog_name(description.database), schema_name(description.schema),
	      table_name(description.table) {
	}

	//! The catalog name.
	string catalog_name;
	//! The schema name.
	string schema_name;
	//! The table name.
	string table_name;
	//! The timestamp/version at which to read this table entry (if any)
	unique_ptr<AtClause> at_clause;

public:
	string ToString() const override;
	bool Equals(const TableRef &other_p) const override;
	unique_ptr<TableRef> Copy() override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};

} // namespace goose
