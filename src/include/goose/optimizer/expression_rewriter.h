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
#include <goose/planner/logical_operator_visitor.h>
#include <goose/common/types/value.h>

namespace goose {
class ClientContext;

//! The ExpressionRewriter performs a set of fixed rewrite rules on the expressions that occur in a SQL statement
class ExpressionRewriter : public LogicalOperatorVisitor {
public:
	explicit ExpressionRewriter(ClientContext &context) : context(context) {
	}

public:
	//! The set of rules as known by the Expression Rewriter
	vector<unique_ptr<Rule>> rules;

	ClientContext &context;

public:
	void VisitOperator(LogicalOperator &op) override;
	void VisitExpression(unique_ptr<Expression> *expression) override;

	// Generates either a constant_or_null(child) expression
	static unique_ptr<Expression> ConstantOrNull(unique_ptr<Expression> child, Value value);
	static unique_ptr<Expression> ConstantOrNull(vector<unique_ptr<Expression>> children, Value value);

private:
	//! Apply a set of rules to a specific expression
	static unique_ptr<Expression> ApplyRules(LogicalOperator &op, const vector<reference<Rule>> &rules,
	                                         unique_ptr<Expression> expr, bool &changes_made, bool is_root = false);

	optional_ptr<LogicalOperator> op;
	vector<reference<Rule>> to_apply_rules;
};

} // namespace goose
