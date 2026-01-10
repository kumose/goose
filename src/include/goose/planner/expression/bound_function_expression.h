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

#include <goose/function/scalar_function.h>
#include <goose/planner/expression.h>

namespace goose {
class ScalarFunctionCatalogEntry;

//! Represents a function call that has been bound to a base function
class BoundFunctionExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_FUNCTION;

public:
	BoundFunctionExpression(LogicalType return_type, ScalarFunction bound_function,
	                        vector<unique_ptr<Expression>> arguments, unique_ptr<FunctionData> bind_info,
	                        bool is_operator = false);

	//! The bound function expression
	ScalarFunction function;
	//! List of child-expressions of the function
	vector<unique_ptr<Expression>> children;
	//! The bound function data (if any)
	unique_ptr<FunctionData> bind_info;
	//! Whether or not the function is an operator, only used for rendering
	bool is_operator;

public:
	bool IsVolatile() const override;
	bool IsConsistent() const override;
	bool IsFoldable() const override;
	bool CanThrow() const override;
	string ToString() const override;
	bool PropagatesNullValues() const override;
	hash_t Hash() const override;
	bool Equals(const BaseExpression &other) const override;

	unique_ptr<Expression> Copy() const override;
	void Verify() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);
};

} // namespace goose
