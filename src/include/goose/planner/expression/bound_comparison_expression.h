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

#include <goose/planner/expression.h>

namespace goose {

class BoundComparisonExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_COMPARISON;

public:
	BoundComparisonExpression(ExpressionType type, unique_ptr<Expression> left, unique_ptr<Expression> right);

	unique_ptr<Expression> left;
	unique_ptr<Expression> right;

public:
	string ToString() const override;

	bool Equals(const BaseExpression &other) const override;

	unique_ptr<Expression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);

public:
	static LogicalType BindComparison(ClientContext &context, const LogicalType &left_type,
	                                  const LogicalType &right_type, ExpressionType comparison_type);
	static bool TryBindComparison(ClientContext &context, const LogicalType &left_type, const LogicalType &right_type,
	                              LogicalType &result_type, ExpressionType comparison_type);
};
} // namespace goose
