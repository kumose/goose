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
