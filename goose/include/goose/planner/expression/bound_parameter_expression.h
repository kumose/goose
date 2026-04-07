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
