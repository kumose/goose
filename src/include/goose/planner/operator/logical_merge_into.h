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
#include <goose/common/enums/merge_action_type.h>
#include <goose/common/index_vector.h>

namespace goose {
class TableCatalogEntry;
class LogicalGet;
class LogicalProjection;

class BoundMergeIntoAction {
public:
	//! The merge action type
	MergeActionType action_type;
	//! Condition - or NULL if this should always be performed for the given action
	unique_ptr<Expression> condition;
	//! The set of referenced physical columns (for UPDATE)
	vector<PhysicalIndex> columns;
	//! Set of expressions for INSERT or UPDATE
	vector<unique_ptr<Expression>> expressions;
	//! Column index map (for INSERT)
	physical_index_vector_t<idx_t> column_index_map;
	//! Whether or not an UPDATE is a DELETE + INSERT
	bool update_is_del_and_insert = false;

	void Serialize(Serializer &serializer) const;
	static unique_ptr<BoundMergeIntoAction> Deserialize(Deserializer &deserializer);
};

class LogicalMergeInto : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_MERGE_INTO;

public:
	explicit LogicalMergeInto(TableCatalogEntry &table);

	//! The base table to merge into
	TableCatalogEntry &table;
	//! projection index
	idx_t table_index;
	vector<unique_ptr<Expression>> bound_defaults;
	idx_t row_id_start;
	optional_idx source_marker;
	//! Bound constraints
	vector<unique_ptr<BoundConstraint>> bound_constraints;
	//! Whether or not to return the input data
	bool return_chunk = false;

	map<MergeActionCondition, vector<unique_ptr<BoundMergeIntoAction>>> actions;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;
	vector<idx_t> GetTableIndex() const override;

protected:
	vector<ColumnBinding> GetColumnBindings() override;
	void ResolveTypes() override;

private:
	LogicalMergeInto(ClientContext &context, const unique_ptr<CreateInfo> &table_info);
};

} // namespace goose
