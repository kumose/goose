// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
