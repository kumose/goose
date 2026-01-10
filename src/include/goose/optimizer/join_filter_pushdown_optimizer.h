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
