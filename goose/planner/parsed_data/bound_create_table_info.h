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
