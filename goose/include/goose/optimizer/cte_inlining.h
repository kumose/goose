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

#include <goose/common/insertion_order_preserving_map.h>
#include <goose/planner/bound_parameter_map.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/expression/list.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/operator/list.h>

namespace goose {

class LogicalOperator;
class Optimizer;
struct BoundParameterData;

class CTEInlining {
public:
	explicit CTEInlining(Optimizer &optimizer);
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);
	static bool EndsInAggregateOrDistinct(const LogicalOperator &op);

private:
	void TryInlining(unique_ptr<LogicalOperator> &op);
	bool Inline(unique_ptr<LogicalOperator> &op, LogicalOperator &materialized_cte, bool requires_copy = true);

private:
	//! The optimizer
	Optimizer &optimizer;

	optional_ptr<bound_parameter_map_t> parameter_data;
};

class PreventInlining : public LogicalOperatorVisitor {
public:
	PreventInlining() : prevent_inlining(false) {};

	void VisitOperator(LogicalOperator &op) override;
	bool prevent_inlining;

	void VisitExpression(unique_ptr<Expression> *expression) override;
};

} // namespace goose
