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

#include <goose/common/types-import.h>
#include <goose/common/vector.h>
#include <goose/planner/logical_operator.h>

namespace goose {

struct BuildSize {
	double left_side;
	double right_side;

	// Initialize with 1 so the build side is just the cardinality if types aren't
	// known.
	BuildSize() : left_side(1), right_side(1) {
	}
};

class BuildProbeSideOptimizer : LogicalOperatorVisitor {
private:
	static constexpr idx_t COLUMN_COUNT_PENALTY = 2;
	static constexpr double PREFER_RIGHT_DEEP_PENALTY = 0.15;

public:
	explicit BuildProbeSideOptimizer(ClientContext &context, LogicalOperator &op);
	void VisitOperator(LogicalOperator &op) override;
	void VisitExpression(unique_ptr<Expression> *expression) override {};

private:
	bool TryFlipJoinChildren(LogicalOperator &op) const;
	static idx_t ChildHasJoins(LogicalOperator &op);

	static BuildSize GetBuildSizes(const LogicalOperator &op, idx_t lhs_cardinality, idx_t rhs_cardinality);
	static double GetBuildSize(vector<LogicalType> types, idx_t cardinality);

private:
	ClientContext &context;
	vector<ColumnBinding> preferred_on_probe_side;
};

} // namespace goose
