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
