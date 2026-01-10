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

#include <goose/common/enums/statement_type.h>
#include <goose/parser/parsed_data/parse_info.h>
#include <goose/planner/logical_operator.h>

namespace goose {

//! LogicalSimple represents a simple logical operator that only passes on the parse info
class LogicalSimple : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	LogicalSimple(LogicalOperatorType type, unique_ptr<ParseInfo> info) : LogicalOperator(type), info(std::move(info)) {
	}

	unique_ptr<ParseInfo> info;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	idx_t EstimateCardinality(ClientContext &context) override;

protected:
	void ResolveTypes() override {
		types.emplace_back(LogicalType::BOOLEAN);
	}
};
} // namespace goose
