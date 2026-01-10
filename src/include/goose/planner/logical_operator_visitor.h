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

#include <goose/common/common.h>
#include <goose/planner/bound_tokens.h>
#include <goose/planner/logical_tokens.h>

#include <functional>

namespace goose {
//! The LogicalOperatorVisitor is an abstract base class that implements the
//! Visitor pattern on LogicalOperator.
class LogicalOperatorVisitor {
public:
	virtual ~LogicalOperatorVisitor() {
	}

	virtual void VisitOperator(LogicalOperator &op);
	virtual void VisitExpression(unique_ptr<Expression> *expression);

	static void EnumerateExpressions(LogicalOperator &op,
	                                 const std::function<void(unique_ptr<Expression> *child)> &callback);

protected:
	//! Automatically calls the Visit method for LogicalOperator children of the current operator. Can be overloaded to
	//! change this behavior.
	void VisitOperatorChildren(LogicalOperator &op);
	//! Automatically calls the Visit method for Expression children of the current operator. Can be overloaded to
	//! change this behavior.
	void VisitOperatorExpressions(LogicalOperator &op);
	//! Alternatives for VisitOperatorChildren for operators that have a projection map
	void VisitOperatorWithProjectionMapChildren(LogicalOperator &op);
	void VisitChildOfOperatorWithProjectionMap(LogicalOperator &child, vector<idx_t> &projection_map);

	// The VisitExpressionChildren method is called at the end of every call to VisitExpression to recursively visit all
	// expressions in an expression tree. It can be overloaded to prevent automatically visiting the entire tree.
	virtual void VisitExpressionChildren(Expression &expression);

	virtual unique_ptr<Expression> VisitReplace(BoundAggregateExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundBetweenExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundCaseExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundCastExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundComparisonExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundConjunctionExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundConstantExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundDefaultExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundFunctionExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundOperatorExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundReferenceExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundSubqueryExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundParameterExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundWindowExpression &expr, unique_ptr<Expression> *expr_ptr);
	virtual unique_ptr<Expression> VisitReplace(BoundUnnestExpression &expr, unique_ptr<Expression> *expr_ptr);
};
} // namespace goose
