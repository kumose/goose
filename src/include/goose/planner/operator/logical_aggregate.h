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

#include <goose/common/enums/tuple_data_layout_enums.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/column_binding.h>
#include <goose/storage/statistics/base_statistics.h>
#include <goose/parser/group_by_node.h>

namespace goose {

//! LogicalAggregate represents an aggregate operation with (optional) GROUP BY
//! operator.
class LogicalAggregate : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_AGGREGATE_AND_GROUP_BY;

public:
	LogicalAggregate(idx_t group_index, idx_t aggregate_index, vector<unique_ptr<Expression>> select_list);

	//! The table index for the groups of the LogicalAggregate
	idx_t group_index;
	//! The table index for the aggregates of the LogicalAggregate
	idx_t aggregate_index;
	//! The table index for the GROUPING function calls of the LogicalAggregate
	idx_t groupings_index;
	//! The set of groups (optional).
	vector<unique_ptr<Expression>> groups;
	//! The set of grouping sets (optional).
	vector<GroupingSet> grouping_sets;
	//! The list of grouping function calls (optional)
	vector<unsafe_vector<idx_t>> grouping_functions;
	//! Group statistics (optional)
	vector<unique_ptr<BaseStatistics>> group_stats;
	//! Whether the inputs to all expression are non-NULL
	TupleDataValidityType distinct_validity;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	vector<ColumnBinding> GetColumnBindings() override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	idx_t EstimateCardinality(ClientContext &context) override;
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override;
};
} // namespace goose
