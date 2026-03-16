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

#include <goose/optimizer/optimizer.h>
#include <goose/common/types-import.h>

namespace goose {
class TableFilterSet;
class TableFilter;

class ExpressionHeuristics : public LogicalOperatorVisitor {
public:
	explicit ExpressionHeuristics(Optimizer &optimizer) : optimizer(optimizer) {
	}

	Optimizer &optimizer;
	unique_ptr<LogicalOperator> root;

public:
	//! Search for filters to be reordered
	unique_ptr<LogicalOperator> Rewrite(unique_ptr<LogicalOperator> op);
	//! Reorder the expressions of a filter
	void ReorderExpressions(vector<unique_ptr<Expression>> &expressions);
	//! Return the cost of an expression
	static idx_t Cost(Expression &expr);

	static vector<idx_t> GetInitialOrder(const TableFilterSet &table_filters);

	unique_ptr<Expression> VisitReplace(BoundConjunctionExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	//! Override this function to search for filter operators
	void VisitOperator(LogicalOperator &op) override;

private:
	static idx_t ExpressionCost(BoundBetweenExpression &expr);
	static idx_t ExpressionCost(BoundCaseExpression &expr);
	static idx_t ExpressionCost(BoundCastExpression &expr);
	static idx_t ExpressionCost(BoundComparisonExpression &expr);
	static idx_t ExpressionCost(BoundConjunctionExpression &expr);
	static idx_t ExpressionCost(BoundFunctionExpression &expr);
	static idx_t ExpressionCost(BoundOperatorExpression &expr, ExpressionType expr_type);
	static idx_t ExpressionCost(PhysicalType return_type, idx_t multiplier);
	static idx_t Cost(TableFilter &filter);
};
} // namespace goose
