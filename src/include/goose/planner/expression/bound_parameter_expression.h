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
#include <goose/planner/bound_parameter_map.h>

namespace goose {

class BoundParameterExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_PARAMETER;

public:
	explicit BoundParameterExpression(const string &identifier);

	string identifier;
	shared_ptr<BoundParameterData> parameter_data;

public:
	//! Invalidate a bound parameter expression - forcing a rebind on any subsequent filters
	GOOSE_API static void Invalidate(Expression &expr);
	//! Invalidate all parameters within an expression
	GOOSE_API static void InvalidateRecursive(Expression &expr);

	bool IsScalar() const override;
	bool HasParameter() const override;
	bool IsFoldable() const override;

	string ToString() const override;

	bool Equals(const BaseExpression &other) const override;
	hash_t Hash() const override;

	unique_ptr<Expression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);

private:
	BoundParameterExpression(bound_parameter_map_t &global_parameter_set, string identifier, LogicalType return_type,
	                         shared_ptr<BoundParameterData> parameter_data);
};

} // namespace goose
