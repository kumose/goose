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

#include <goose/parser/column_definition.h>

namespace goose {

class TableDescription {
public:
	TableDescription(const string &database_name, const string &schema_name, const string &table_name)
	    : database(database_name), schema(schema_name), table(table_name) {};

	TableDescription() = delete;

public:
	//! The database of the table.
	string database;
	//! The schema of the table.
	string schema;
	//! The name of the table.
	string table;
	//! True, if the catalog is readonly.
	bool readonly;
	//! The columns of the table.
	vector<ColumnDefinition> columns;

public:
	idx_t PhysicalColumnCount() const {
		idx_t count = 0;
		for (auto &column : columns) {
			if (column.Generated()) {
				continue;
			}
			count++;
		}
		D_ASSERT(count != 0);
		return count;
	}
};

} // namespace goose
