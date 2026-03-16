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

#include <goose/common/enums/subquery_type.h>
#include <goose/planner/binder.h>
#include <goose/planner/bound_query_node.h>
#include <goose/planner/expression.h>

namespace goose {

class BoundSubqueryExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_SUBQUERY;

public:
	explicit BoundSubqueryExpression(LogicalType return_type);

	bool IsCorrelated() const {
		return !binder->correlated_columns.empty();
	}

	//! The binder used to bind the subquery node
	shared_ptr<Binder> binder;
	//! The bound subquery node
	BoundStatement subquery;
	//! The subquery type
	SubqueryType subquery_type;
	//! the child expressions to compare with (in case of IN, ANY, ALL operators)
	vector<unique_ptr<Expression>> children;
	//! The comparison type of the child expression with the subquery (in case of ANY, ALL operators)
	ExpressionType comparison_type;
	//! The LogicalTypes of the subquery result. Only used for ANY expressions.
	vector<LogicalType> child_types;
	//! The target LogicalType of the subquery result (i.e. to which type it should be casted, if child_type <>
	//! child_target). Only used for ANY expressions.
	vector<LogicalType> child_targets;

public:
	bool HasSubquery() const override {
		return true;
	}
	bool IsScalar() const override {
		return false;
	}
	bool IsFoldable() const override {
		return false;
	}

	string ToString() const override;

	bool Equals(const BaseExpression &other) const override;

	unique_ptr<Expression> Copy() const override;

	bool PropagatesNullValues() const override;
};
} // namespace goose
