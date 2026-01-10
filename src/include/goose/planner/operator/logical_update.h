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
#include <goose/planner/bound_constraint.h>
#include <goose/common/index_map.h>

namespace goose {
class TableCatalogEntry;
class LogicalGet;
class LogicalProjection;

class LogicalUpdate : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_UPDATE;

public:
	explicit LogicalUpdate(TableCatalogEntry &table);

	//! The base table to update
	TableCatalogEntry &table;
	//! projection index
	idx_t table_index;
	//! if returning option is used, return the update chunk
	bool return_chunk;
	vector<PhysicalIndex> columns;
	vector<unique_ptr<Expression>> bound_defaults;
	vector<unique_ptr<BoundConstraint>> bound_constraints;
	bool update_is_del_and_insert;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;
	string GetName() const override;

	GOOSE_API static void BindExtraColumns(TableCatalogEntry &table, LogicalGet &get, LogicalProjection &proj,
	                                        LogicalUpdate &update, physical_index_set_t &bound_columns);

protected:
	vector<ColumnBinding> GetColumnBindings() override;
	void ResolveTypes() override;

private:
	LogicalUpdate(ClientContext &context, const unique_ptr<CreateInfo> &table_info);
};
} // namespace goose
