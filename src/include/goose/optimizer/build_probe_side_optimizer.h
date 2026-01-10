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
