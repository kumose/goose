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

#include <goose/function/table_function.h>
#include <goose/common/types-import.h>
#include <goose/function/built_in_functions.h>

namespace goose {
class GooseTableEntry;
class TableCatalogEntry;

struct TableScanBindData : public TableFunctionData {
	explicit TableScanBindData(TableCatalogEntry &table) : table(table), is_index_scan(false), is_create_index(false) {
	}

	//! The table to scan.
	TableCatalogEntry &table;
	//! The old purpose of this field has been deprecated.
	//! We now use it to express an index scan in the ANALYZE call.
	//! I.e., we const-cast the bind data and set this to true, if we opt for an index scan.
	bool is_index_scan;
	//! Whether or not the table scan is for index creation.
	bool is_create_index;
	//! In what order to scan the row groups
	unique_ptr<RowGroupOrderOptions> order_options;

public:
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<TableScanBindData>();
		return &other.table == &table;
	}
	unique_ptr<FunctionData> Copy() const override {
		auto bind_data = make_uniq<TableScanBindData>(table);
		bind_data->is_index_scan = is_index_scan;
		bind_data->is_create_index = is_create_index;
		bind_data->column_ids = column_ids;
		return std::move(bind_data);
	}
};

//! The table scan function represents a sequential or index scan over one of Goose's base tables.
struct TableScanFunction {
	static void RegisterFunction(BuiltinFunctions &set);
	static TableFunction GetFunction();
};

} // namespace goose
