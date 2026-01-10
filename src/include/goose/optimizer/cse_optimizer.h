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

#include <goose/optimizer/rule.h>
#include <goose/parser/expression_map.h>
#include <goose/planner/logical_operator_visitor.h>

namespace goose {
class Binder;
struct CSEReplacementState;

//! The CommonSubExpression optimizer traverses the expressions of a LogicalOperator to look for duplicate expressions
//! if there are any, it pushes a projection under the operator that resolves these expressions
class CommonSubExpressionOptimizer : public LogicalOperatorVisitor {
public:
	explicit CommonSubExpressionOptimizer(Binder &binder) : binder(binder) {
	}

public:
	void VisitOperator(LogicalOperator &op) override;

private:
	//! First iteration: count how many times each expression occurs
	void CountExpressions(Expression &expr, CSEReplacementState &state);
	//! Second iteration: perform the actual replacement of the duplicate expressions with common subexpressions nodes
	void PerformCSEReplacement(unique_ptr<Expression> &expr, CSEReplacementState &state);

	//! Main method to extract common subexpressions
	void ExtractCommonSubExpresions(LogicalOperator &op);

private:
	Binder &binder;
};
} // namespace goose
