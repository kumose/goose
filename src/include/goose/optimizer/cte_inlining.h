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

#include <goose/common/insertion_order_preserving_map.h>
#include <goose/planner/bound_parameter_map.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/expression/list.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/operator/list.h>

namespace goose {

class LogicalOperator;
class Optimizer;
struct BoundParameterData;

class CTEInlining {
public:
	explicit CTEInlining(Optimizer &optimizer);
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);
	static bool EndsInAggregateOrDistinct(const LogicalOperator &op);

private:
	void TryInlining(unique_ptr<LogicalOperator> &op);
	bool Inline(unique_ptr<LogicalOperator> &op, LogicalOperator &materialized_cte, bool requires_copy = true);

private:
	//! The optimizer
	Optimizer &optimizer;

	optional_ptr<bound_parameter_map_t> parameter_data;
};

class PreventInlining : public LogicalOperatorVisitor {
public:
	PreventInlining() : prevent_inlining(false) {};

	void VisitOperator(LogicalOperator &op) override;
	bool prevent_inlining;

	void VisitExpression(unique_ptr<Expression> *expression) override;
};

} // namespace goose
