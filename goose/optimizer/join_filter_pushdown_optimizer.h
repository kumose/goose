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

#include <goose/planner/logical_operator_visitor.h>
#include <goose/planner/column_binding_map.h>

namespace goose {
class Optimizer;
struct JoinFilterPushdownColumn;
struct PushdownFilterTarget;

//! The JoinFilterPushdownOptimizer links comparison joins to data sources to enable dynamic execution-time filter
//! pushdown
class JoinFilterPushdownOptimizer : public LogicalOperatorVisitor {
public:
	explicit JoinFilterPushdownOptimizer(Optimizer &optimizer);

public:
	void VisitOperator(LogicalOperator &op) override;
	static void GetPushdownFilterTargets(LogicalOperator &op, vector<JoinFilterPushdownColumn> columns,
	                                     vector<PushdownFilterTarget> &targets);

	static bool IsFiltering(const unique_ptr<LogicalOperator> &op);

private:
	void GenerateJoinFilters(LogicalComparisonJoin &join);

private:
	Optimizer &optimizer;
};
} // namespace goose
