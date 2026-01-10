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

#include <goose/parser/parsed_expression.h>
#include <goose/planner/operator/logical_join.h>

namespace goose {

//! LogicalAnyJoin represents a join with an arbitrary expression as JoinCondition
class LogicalAnyJoin : public LogicalJoin {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_ANY_JOIN;

public:
	explicit LogicalAnyJoin(JoinType type);

	//! The JoinCondition on which this join is performed
	unique_ptr<Expression> condition;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
};

} // namespace goose
