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
