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
