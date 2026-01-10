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
