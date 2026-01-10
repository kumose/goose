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
#include <goose/parser/parsed_data/create_info.h>

namespace goose {

//! LogicalCreate represents a CREATE operator
class LogicalCreate : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	LogicalCreate(LogicalOperatorType type, unique_ptr<CreateInfo> info,
	              optional_ptr<SchemaCatalogEntry> schema = nullptr);

	optional_ptr<SchemaCatalogEntry> schema;
	unique_ptr<CreateInfo> info;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;

protected:
	void ResolveTypes() override;

private:
	LogicalCreate(LogicalOperatorType type, ClientContext &context, unique_ptr<CreateInfo> info);
};
} // namespace goose
