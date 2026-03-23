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
