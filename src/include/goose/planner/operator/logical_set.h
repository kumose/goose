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

#include <goose/common/enums/set_scope.h>
#include <goose/parser/parsed_data/copy_info.h>
#include <goose/planner/logical_operator.h>
#include <goose/function/copy_function.h>

namespace goose {

class LogicalSet : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_SET;

public:
	LogicalSet(std::string name_p, Value value_p, SetScope scope_p)
	    : LogicalOperator(LogicalOperatorType::LOGICAL_SET), name(std::move(name_p)), value(std::move(value_p)),
	      scope(scope_p) {
	}

	std::string name;
	Value value;
	SetScope scope;

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
