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
