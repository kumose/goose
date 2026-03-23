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

#include <goose/parser/parsed_data/create_index_info.h>
#include <goose/planner/logical_operator.h>
#include <goose/function/table_function.h>

namespace goose {

class LogicalCreateIndex : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_CREATE_INDEX;

public:
	LogicalCreateIndex(unique_ptr<CreateIndexInfo> info_p, vector<unique_ptr<Expression>> expressions_p,
	                   TableCatalogEntry &table_p, unique_ptr<AlterTableInfo> alter_table_info = nullptr);

	//! Index creation information.
	unique_ptr<CreateIndexInfo> info;
	//! The table to create the index for.
	TableCatalogEntry &table;
	// Alter table information.
	unique_ptr<AlterTableInfo> alter_table_info;
	//! Unbound expressions of the indexed columns.
	vector<unique_ptr<Expression>> unbound_expressions;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	void ResolveTypes() override;

private:
	LogicalCreateIndex(ClientContext &context, unique_ptr<CreateInfo> info, vector<unique_ptr<Expression>> expressions,
	                   unique_ptr<ParseInfo> alter_info);
	TableCatalogEntry &BindTable(ClientContext &context, CreateIndexInfo &info_p);
};
} // namespace goose
