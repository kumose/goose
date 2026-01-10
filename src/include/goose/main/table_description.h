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
