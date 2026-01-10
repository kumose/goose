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

#include <goose/planner/parsed_data/bound_create_table_info.h>
#include <goose/planner/logical_operator.h>

namespace goose {

class LogicalCreateTable : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_CREATE_TABLE;

public:
	LogicalCreateTable(SchemaCatalogEntry &schema, unique_ptr<BoundCreateTableInfo> info);

	//! Schema to insert to
	SchemaCatalogEntry &schema;
	//! Create Table information
	unique_ptr<BoundCreateTableInfo> info;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;

protected:
	void ResolveTypes() override;

private:
	LogicalCreateTable(ClientContext &context, unique_ptr<CreateInfo> info);
};
} // namespace goose
