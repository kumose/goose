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

#include <goose/planner/logical_operator.h>
#include <goose/common/index_vector.h>
#include <goose/parser/statement/insert_statement.h>

namespace goose {
class TableCatalogEntry;

class Index;

struct BoundOnConflictInfo {
	BoundOnConflictInfo();

	//! Which action to take on conflict
	OnConflictAction action_type;
	// The types that the DO UPDATE .. SET (expressions) are cast to
	vector<LogicalType> expected_set_types;
	// The (distinct) column ids to apply the ON CONFLICT on
	unordered_set<column_t> on_conflict_filter;
	// The WHERE clause of the conflict_target (ON CONFLICT .. WHERE <condition>)
	unique_ptr<Expression> on_conflict_condition;
	// The WHERE clause of the DO UPDATE clause
	unique_ptr<Expression> do_update_condition;
	// The columns targeted by the DO UPDATE SET expressions
	vector<PhysicalIndex> set_columns;
	// The types of the columns targeted by the DO UPDATE SET expressions
	vector<LogicalType> set_types;
	// The table_index referring to the column references qualified with 'excluded'
	idx_t excluded_table_index = 0;
	// The columns to fetch from the 'destination' table
	vector<column_t> columns_to_fetch;
	// The columns to fetch from the 'source' table
	vector<column_t> source_columns;
	//! True, if the INSERT OR REPLACE requires delete + insert.
	bool update_is_del_and_insert;
};

//! LogicalInsert represents an insertion of data into a base table
class LogicalInsert : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INSERT;

public:
	LogicalInsert(TableCatalogEntry &table, idx_t table_index);

	vector<vector<unique_ptr<Expression>>> insert_values;
	//! The insertion map ([table_index -> index in result, or DConstants::INVALID_INDEX if not specified])
	physical_index_vector_t<idx_t> column_index_map;
	//! The expected types for the INSERT statement (obtained from the column types)
	vector<LogicalType> expected_types;
	//! The base table to insert into
	TableCatalogEntry &table;
	idx_t table_index;
	//! if returning option is used, return actual chunk to projection
	bool return_chunk;
	//! The default statements used by the table
	vector<unique_ptr<Expression>> bound_defaults;
	//! The constraints used by the table
	vector<unique_ptr<BoundConstraint>> bound_constraints;

	BoundOnConflictInfo on_conflict_info;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	vector<ColumnBinding> GetColumnBindings() override;
	void ResolveTypes() override;

	idx_t EstimateCardinality(ClientContext &context) override;
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

private:
	LogicalInsert(ClientContext &context, const unique_ptr<CreateInfo> table_info);
};
} // namespace goose
