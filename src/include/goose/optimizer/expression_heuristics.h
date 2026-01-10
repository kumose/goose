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
