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
#include <goose/function/cast/default_casts.h>

namespace goose {

class BoundCastExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_CAST;

public:
	BoundCastExpression(unique_ptr<Expression> child, LogicalType target_type, BoundCastInfo bound_cast,
	                    bool try_cast = false);

	//! The child type
	unique_ptr<Expression> child;
	//! Whether to use try_cast or not. try_cast converts cast failures into NULLs instead of throwing an error.
	bool try_cast;
	//! The bound cast info
	BoundCastInfo bound_cast;

public:
	LogicalType source_type() { // NOLINT: allow casing for legacy reasons
		D_ASSERT(child->return_type.IsValid());
		return child->return_type;
	}

	//! Cast an expression to the specified SQL type, using only the built-in SQL casts
	static unique_ptr<Expression> AddDefaultCastToType(unique_ptr<Expression> expr, const LogicalType &target_type,
	                                                   bool try_cast = false);
	//! Cast an expression to the specified SQL type if required
	GOOSE_API static unique_ptr<Expression> AddCastToType(ClientContext &context, unique_ptr<Expression> expr,
	                                                       const LogicalType &target_type, bool try_cast = false);

	//! If the expression returns an array, cast it to return a list with the same child type. Otherwise do nothing.
	GOOSE_API static unique_ptr<Expression> AddArrayCastToList(ClientContext &context, unique_ptr<Expression> expr);

	//! Returns true if a cast is invertible (i.e. CAST(s -> t -> s) = s for all values of s). This is not true for e.g.
	//! boolean casts, because that can be e.g. -1 -> TRUE -> 1. This is necessary to prevent some optimizer bugs.
	static bool CastIsInvertible(const LogicalType &source_type, const LogicalType &target_type);

	string ToString() const override;

	bool Equals(const BaseExpression &other) const override;

	unique_ptr<Expression> Copy() const override;

	bool CanThrow() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);

private:
	BoundCastExpression(ClientContext &context, unique_ptr<Expression> child, LogicalType target_type);
};
} // namespace goose
