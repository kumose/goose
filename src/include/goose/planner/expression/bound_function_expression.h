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
