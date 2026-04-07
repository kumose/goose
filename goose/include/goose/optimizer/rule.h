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

#include <goose/optimizer/matcher/expression_matcher.h>
#include <goose/optimizer/matcher/logical_operator_matcher.h>

namespace goose {
class ExpressionRewriter;

class Rule {
public:
	explicit Rule(ExpressionRewriter &rewriter) : rewriter(rewriter) {
	}
	virtual ~Rule() {
	}

	//! The expression rewriter this rule belongs to
	ExpressionRewriter &rewriter;
	//! The expression matcher of the rule
	unique_ptr<ExpressionMatcher> root;

	ClientContext &GetContext() const;
	virtual unique_ptr<Expression> Apply(LogicalOperator &op, vector<reference<Expression>> &bindings,
	                                     bool &fixed_point, bool is_root) = 0;
};

} // namespace goose
