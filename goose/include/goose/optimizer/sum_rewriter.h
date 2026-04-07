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

#include <goose/planner/column_binding_map.h>
#include <goose/planner/logical_operator_visitor.h>

namespace goose {
class ExpressionMatcher;
class Optimizer;

//! Rewrites SUM(x + C) into SUM(x) + C * COUNT(x)
class SumRewriterOptimizer : public LogicalOperatorVisitor {
public:
	explicit SumRewriterOptimizer(Optimizer &optimizer);
	~SumRewriterOptimizer() override;

	void Optimize(unique_ptr<LogicalOperator> &op);
	void VisitOperator(LogicalOperator &op) override;

private:
	void StandardVisitOperator(LogicalOperator &op);
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	void RewriteSums(unique_ptr<LogicalOperator> &aggr);

private:
	Optimizer &optimizer;
	column_binding_map_t<ColumnBinding> aggregate_map;
	unique_ptr<ExpressionMatcher> sum_matcher;
};
} // namespace goose
