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
