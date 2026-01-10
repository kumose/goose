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
#include <goose/parser/expression/lambda_expression.h>

namespace goose {

class BoundLambdaExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_LAMBDA;

public:
	BoundLambdaExpression(ExpressionType type_p, LogicalType return_type_p, unique_ptr<Expression> lambda_expr_p,
	                      idx_t parameter_count_p);

	//! The lambda expression that we'll use in the expression executor during execution
	unique_ptr<Expression> lambda_expr;
	//! Non-lambda constants, column references, and outer lambda parameters that we need to pass
	//! into the execution chunk
	vector<unique_ptr<Expression>> captures;
	//! The number of lhs parameters of the lambda function
	idx_t parameter_count;

public:
	string ToString() const override;
	bool Equals(const BaseExpression &other) const override;
	unique_ptr<Expression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);
};
} // namespace goose
