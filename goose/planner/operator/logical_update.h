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
