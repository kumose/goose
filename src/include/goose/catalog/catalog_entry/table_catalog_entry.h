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
#include <goose/common/types-import.h>
#include <goose/parser/column_list.h>
#include <goose/parser/constraint.h>
#include <goose/planner/bound_constraint.h>
#include <goose/storage/table/table_statistics.h>
#include <goose/planner/expression.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/catalog/catalog_entry/table_column_type.h>
#include <goose/catalog/catalog_entry/column_dependency_manager.h>
#include <goose/common/table_column.h>

namespace goose {

class DataTable;

struct RenameColumnInfo;
struct RenameFieldInfo;
struct AddColumnInfo;
struct AddFieldInfo;
struct RemoveColumnInfo;
struct RemoveFieldInfo;
struct SetDefaultInfo;
struct ChangeColumnTypeInfo;
struct AlterForeignKeyInfo;
struct SetNotNullInfo;
struct DropNotNullInfo;
struct SetColumnCommentInfo;
struct CreateTableInfo;
struct BoundCreateTableInfo;

class TableFunction;
struct FunctionData;
struct EntryLookupInfo;

class Binder;
struct ColumnSegmentInfo;
class TableStorageInfo;

class LogicalGet;
class LogicalProjection;
class LogicalUpdate;

//! A table catalog entry
class TableCatalogEntry : public StandardEntry {
public:
	static constexpr const CatalogType Type = CatalogType::TABLE_ENTRY;
	static constexpr const char *Name = "table";

public:
	//! Create a TableCatalogEntry and initialize storage for it
	GOOSE_API TableCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateTableInfo &info);

public:
	GOOSE_API unique_ptr<CreateInfo> GetInfo() const override;

	GOOSE_API bool HasGeneratedColumns() const;

	//! Returns whether or not a column with the given name exists
	GOOSE_API bool ColumnExists(const string &name) const;
	//! Returns a reference to the column of the specified name. Throws an
	//! exception if the column does not exist.
	GOOSE_API const ColumnDefinition &GetColumn(const string &name) const;
	//! Returns a reference to the column of the specified logical index. Throws an
	//! exception if the column does not exist.
	GOOSE_API const ColumnDefinition &GetColumn(LogicalIndex idx) const;
	//! Returns a list of types of the table, excluding generated columns
	GOOSE_API vector<LogicalType> GetTypes() const;
	//! Returns a list of the columns of the table
	GOOSE_API const ColumnList &GetColumns() const;
	//! Returns the underlying storage of the table
	virtual DataTable &GetStorage();

	//! Returns a list of the constraints of the table
	GOOSE_API const vector<unique_ptr<Constraint>> &GetConstraints() const;
	GOOSE_API string ToSQL() const override;

	//! Get statistics of a column (physical or virtual) within the table
	virtual unique_ptr<BaseStatistics> GetStatistics(ClientContext &context, column_t column_id) = 0;

	virtual unique_ptr<BlockingSample> GetSample();

	//! Returns the column index of the specified column name.
	//! If the column does not exist:
	//! If if_column_exists is true, returns DConstants::INVALID_INDEX
	//! If if_column_exists is false, throws an exception
	GOOSE_API LogicalIndex GetColumnIndex(string &name, bool if_exists = false) const;
	GOOSE_API StorageIndex GetStorageIndex(const ColumnIndex &column_index) const;

	//! Returns the scan function that can be used to scan the given table
	virtual TableFunction GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data) = 0;
	virtual TableFunction GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data,
	                                      const EntryLookupInfo &lookup_info);

	virtual bool is_goose_table() const {
		return false;
	}

	GOOSE_API static string ColumnsToSQL(const ColumnList &columns, const vector<unique_ptr<Constraint>> &constraints);

	//! Returns the expression string list of the column names e.g. (col1, col2, col3)
	static string ColumnNamesToSQL(const ColumnList &columns);

	//! Returns a list of segment information for this table, if exists
	virtual vector<ColumnSegmentInfo> GetColumnSegmentInfo(const QueryContext &context);

	//! Returns the storage info of this table
	virtual TableStorageInfo GetStorageInfo(ClientContext &context) = 0;

	virtual void BindUpdateConstraints(Binder &binder, LogicalGet &get, LogicalProjection &proj, LogicalUpdate &update,
	                                   ClientContext &context);

	//! Returns a pointer to the table's primary key, if exists, else nullptr.
	optional_ptr<Constraint> GetPrimaryKey() const;
	//! Returns true, if the table has a primary key, else false.
	bool HasPrimaryKey() const;

	//! Returns the virtual columns for this table
	virtual virtual_column_map_t GetVirtualColumns() const;

	virtual vector<column_t> GetRowIdColumns() const;

protected:
	//! A list of columns that are part of this table
	ColumnList columns;
	//! A list of constraints that are part of this table
	vector<unique_ptr<Constraint>> constraints;
};
} // namespace goose
