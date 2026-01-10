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

#include <goose/common/exception.h>
#include <goose/parser/parsed_expression.h>
#include <goose/planner/expression.h>

namespace goose {

//! BoundExpression is an intermediate dummy class used by the binder. It is a ParsedExpression but holds an Expression.
//! It represents a successfully bound expression. It is used in the Binder to prevent re-binding of already bound parts
//! when dealing with subqueries.
class BoundExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_EXPRESSION;

public:
	explicit BoundExpression(unique_ptr<Expression> expr);

	unique_ptr<Expression> expr;

public:
	static unique_ptr<Expression> &GetExpression(ParsedExpression &expr);

	string ToString() const override;

	bool Equals(const BaseExpression &other) const override;
	hash_t Hash() const override;

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
};

} // namespace goose
