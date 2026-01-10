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
#include <goose/function/aggregate_function.h>
#include <memory>

namespace goose {

class BoundAggregateExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_AGGREGATE;

public:
	BoundAggregateExpression(AggregateFunction function, vector<unique_ptr<Expression>> children,
	                         unique_ptr<Expression> filter, unique_ptr<FunctionData> bind_info,
	                         AggregateType aggr_type);

	//! The bound function expression
	AggregateFunction function;
	//! List of arguments to the function
	vector<unique_ptr<Expression>> children;
	//! The bound function data (if any)
	unique_ptr<FunctionData> bind_info;
	//! The aggregate type (distinct or non-distinct)
	AggregateType aggr_type;

	//! Filter for this aggregate
	unique_ptr<Expression> filter;
	//! The order by expression for this aggregate - if any
	unique_ptr<BoundOrderModifier> order_bys;

public:
	bool IsDistinct() const {
		return aggr_type == AggregateType::DISTINCT;
	}

	bool IsAggregate() const override {
		return true;
	}
	bool IsFoldable() const override {
		return false;
	}
	bool PropagatesNullValues() const override;

	string ToString() const override;

	hash_t Hash() const override;
	bool Equals(const BaseExpression &other) const override;
	unique_ptr<Expression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);
};
} // namespace goose
