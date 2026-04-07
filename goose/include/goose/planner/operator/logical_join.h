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

#include <goose/common/enums/join_type.h>
#include <goose/common/types-import.h>
#include <goose/planner/logical_operator.h>
#include <goose/storage/statistics/base_statistics.h>

namespace goose {

//! LogicalJoin represents a join between two relations
class LogicalJoin : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	explicit LogicalJoin(JoinType type, LogicalOperatorType logical_type = LogicalOperatorType::LOGICAL_JOIN);

public:
	//! Gets the set of table references that are reachable from this node
	static void GetTableReferences(LogicalOperator &op, unordered_set<idx_t> &bindings);
	static void GetExpressionBindings(const Expression &expr, unordered_set<idx_t> &bindings);

	bool HasProjectionMap() const override {
		return !left_projection_map.empty() || !right_projection_map.empty();
	}

	//! The type of the join (INNER, OUTER, etc...)
	JoinType join_type;
	//! Table index used to refer to the MARK column (in case of a MARK join)
	idx_t mark_index {};
	//! The columns of the LHS that are output by the join
	vector<idx_t> left_projection_map;
	//! The columns of the RHS that are output by the join
	vector<idx_t> right_projection_map;
	//! Join Keys statistics (optional)
	vector<unique_ptr<BaseStatistics>> join_stats;

public:
	vector<ColumnBinding> GetColumnBindings() override;
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override;
};

} // namespace goose
