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

#include <goose/planner/operator/logical_unconditional_join.h>

namespace goose {

//! LogicalCrossProduct represents a cross product between two relations
class LogicalCrossProduct : public LogicalUnconditionalJoin {
	LogicalCrossProduct() : LogicalUnconditionalJoin(LogicalOperatorType::LOGICAL_CROSS_PRODUCT) {};

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_CROSS_PRODUCT;

public:
	LogicalCrossProduct(unique_ptr<LogicalOperator> left, unique_ptr<LogicalOperator> right);

public:
	static unique_ptr<LogicalOperator> Create(unique_ptr<LogicalOperator> left, unique_ptr<LogicalOperator> right);

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
};
} // namespace goose
