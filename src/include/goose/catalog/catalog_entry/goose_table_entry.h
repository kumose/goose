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

#include <goose/catalog/catalog_entry/table_catalog_entry.h>
#include <goose/parser/constraints/unique_constraint.h>
#include <goose/planner/constraints/bound_unique_constraint.h>

namespace goose {

struct AddConstraintInfo;

//! A table catalog entry
class GooseTableEntry : public TableCatalogEntry {
public:
	//! Create a TableCatalogEntry and initialize storage for it
	GooseTableEntry(Catalog &catalog, SchemaCatalogEntry &schema, BoundCreateTableInfo &info,
	               shared_ptr<DataTable> inherited_storage = nullptr);

public:
	unique_ptr<CatalogEntry> AlterEntry(ClientContext &context, AlterInfo &info) override;
	unique_ptr<CatalogEntry> AlterEntry(CatalogTransaction, AlterInfo &info) override;
	void UndoAlter(ClientContext &context, AlterInfo &info) override;
	void Rollback(CatalogEntry &prev_entry) override;
	void OnDrop() override;

	//! Returns the underlying storage of the table
	DataTable &GetStorage() override;

	//! Get statistics of a column (physical or virtual) within the table
	unique_ptr<BaseStatistics> GetStatistics(ClientContext &context, const StorageIndex &storage_index);
	unique_ptr<BaseStatistics> GetStatistics(ClientContext &context, column_t column_id) override;

	unique_ptr<BlockingSample> GetSample() override;

	unique_ptr<CatalogEntry> Copy(ClientContext &context) const override;

	void SetAsRoot() override;

	void CommitAlter(string &column_name);
	void CommitDrop();

	TableFunction GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data) override;

	vector<ColumnSegmentInfo> GetColumnSegmentInfo(const QueryContext &context) override;

	TableStorageInfo GetStorageInfo(ClientContext &context) override;

	bool is_goose_table() const override {
		return true;
	}

private:
	unique_ptr<CatalogEntry> RenameColumn(ClientContext &context, RenameColumnInfo &info);
	unique_ptr<CatalogEntry> RenameField(ClientContext &context, RenameFieldInfo &info);
	unique_ptr<CatalogEntry> AddColumn(ClientContext &context, AddColumnInfo &info);
	unique_ptr<CatalogEntry> AddField(ClientContext &context, AddFieldInfo &info);
	unique_ptr<CatalogEntry> RemoveColumn(ClientContext &context, RemoveColumnInfo &info);
	unique_ptr<CatalogEntry> RemoveField(ClientContext &context, RemoveFieldInfo &info);
	unique_ptr<CatalogEntry> SetDefault(ClientContext &context, SetDefaultInfo &info);
	unique_ptr<CatalogEntry> ChangeColumnType(ClientContext &context, ChangeColumnTypeInfo &info);
	unique_ptr<CatalogEntry> SetNotNull(ClientContext &context, SetNotNullInfo &info);
	unique_ptr<CatalogEntry> DropNotNull(ClientContext &context, DropNotNullInfo &info);
	unique_ptr<CatalogEntry> AddForeignKeyConstraint(AlterForeignKeyInfo &info);
	unique_ptr<CatalogEntry> DropForeignKeyConstraint(ClientContext &context, AlterForeignKeyInfo &info);
	unique_ptr<CatalogEntry> SetColumnComment(ClientContext &context, SetColumnCommentInfo &info);
	unique_ptr<CatalogEntry> AddConstraint(ClientContext &context, AddConstraintInfo &info);

	void UpdateConstraintsOnColumnDrop(const LogicalIndex &removed_index, const vector<LogicalIndex> &adjusted_indices,
	                                   const RemoveColumnInfo &info, CreateTableInfo &create_info,
	                                   const vector<unique_ptr<BoundConstraint>> &bound_constraints, bool is_generated);

private:
	//! A reference to the underlying storage unit used for this table
	shared_ptr<DataTable> storage;
	//! Manages dependencies of the individual columns of the table
	ColumnDependencyManager column_dependency_manager;
};
} // namespace goose
