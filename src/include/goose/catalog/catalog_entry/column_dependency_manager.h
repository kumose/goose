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

#include <goose/common/case_insensitive_map.h>
#include <goose/common/string.h>
#include <goose/parser/column_list.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/index_map.h>

namespace goose {

//! Dependency Manager local to a table, responsible for keeping track of generated column dependencies

class ColumnDependencyManager {
public:
	GOOSE_API ColumnDependencyManager();
	GOOSE_API ~ColumnDependencyManager();
	ColumnDependencyManager(ColumnDependencyManager &&other) = default;
	ColumnDependencyManager(const ColumnDependencyManager &other) = delete;

public:
	//! Get the bind order that ensures dependencies are resolved before dependents are
	stack<LogicalIndex> GetBindOrder(const ColumnList &columns);

	//! Adds a connection between the dependent and its dependencies
	void AddGeneratedColumn(LogicalIndex index, const vector<LogicalIndex> &indices, bool root = true);
	//! Add a generated column from a column definition
	void AddGeneratedColumn(const ColumnDefinition &column, const ColumnList &list);

	//! Removes the column(s) and outputs the new column indices
	vector<LogicalIndex> RemoveColumn(LogicalIndex index, idx_t column_amount);

	bool IsDependencyOf(LogicalIndex dependent, LogicalIndex dependency) const;
	bool HasDependencies(LogicalIndex index) const;
	const logical_index_set_t &GetDependencies(LogicalIndex index) const;

	bool HasDependents(LogicalIndex index) const;
	const logical_index_set_t &GetDependents(LogicalIndex index) const;

private:
	void RemoveStandardColumn(LogicalIndex index);
	void RemoveGeneratedColumn(LogicalIndex index);

	void AdjustSingle(LogicalIndex idx, idx_t offset);
	// Clean up the gaps created by a Remove operation
	vector<LogicalIndex> CleanupInternals(idx_t column_amount);

private:
	//! A map of column dependency to generated column(s)
	logical_index_map_t<logical_index_set_t> dependencies_map;
	//! A map of generated column name to (potentially generated)column dependencies
	logical_index_map_t<logical_index_set_t> dependents_map;
	//! For resolve-order purposes, keep track of the 'direct' (not inherited) dependencies of a generated column
	logical_index_map_t<logical_index_set_t> direct_dependencies;
	logical_index_set_t deleted_columns;
};

} // namespace goose
