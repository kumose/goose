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

namespace goose {

// (X AND B) OR (X AND C) OR (X AND D) = X AND (B OR C OR D)
class DistributivityRule : public Rule {
public:
	explicit DistributivityRule(ExpressionRewriter &rewriter);

	unique_ptr<Expression> Apply(LogicalOperator &op, vector<reference<Expression>> &bindings, bool &changes_made,
	                             bool is_root) override;

private:
	void AddExpressionSet(Expression &expr, expression_set_t &set);
	unique_ptr<Expression> ExtractExpression(BoundConjunctionExpression &conj, idx_t idx, Expression &expr);
};

} // namespace goose
