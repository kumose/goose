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

#include <goose/parser/parsed_data/create_table_info.h>
#include <goose/planner/bound_constraint.h>
#include <goose/planner/expression.h>
#include <goose/planner/logical_operator.h>
#include <goose/storage/table/persistent_table_data.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/catalog/catalog_entry/table_column_type.h>
#include <goose/catalog/catalog_entry/column_dependency_manager.h>
#include <goose/catalog/dependency_list.h>
#include <goose/storage/table_storage_info.h>

namespace goose {
class CatalogEntry;

struct BoundCreateTableInfo {
	explicit BoundCreateTableInfo(SchemaCatalogEntry &schema, unique_ptr<CreateInfo> base_p)
	    : schema(schema), base(std::move(base_p)) {
		D_ASSERT(base);
	}

	//! The schema to create the table in
	SchemaCatalogEntry &schema;
	//! The base CreateInfo object
	unique_ptr<CreateInfo> base;
	//! Column dependency manager of the table
	ColumnDependencyManager column_dependency_manager;
	//! List of constraints on the table
	vector<unique_ptr<Constraint>> constraints;
	//! Dependents of the table (in e.g. default values)
	LogicalDependencyList dependencies;
	//! The existing table data on disk (if any)
	unique_ptr<PersistentTableData> data;
	//! CREATE TABLE from QUERY
	unique_ptr<LogicalOperator> query;
	//! Indexes created by this table
	vector<IndexStorageInfo> indexes;

	CreateTableInfo &Base() {
		D_ASSERT(base);
		return base->Cast<CreateTableInfo>();
	}
};

} // namespace goose
