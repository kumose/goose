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

#include <goose/execution/expression_executor.h>
#include <goose/function/aggregate_function.h>
#include <goose/parser/group_by_node.h>
#include <goose/planner/expression/bound_aggregate_expression.h>

namespace goose {

class GroupedAggregateData {
public:
	GroupedAggregateData() {
	}
	//! The groups
	vector<unique_ptr<Expression>> groups;
	//! The set of GROUPING functions
	vector<vector<idx_t>> grouping_functions;
	//! The group types
	vector<LogicalType> group_types;

	//! The aggregates that have to be computed
	vector<unique_ptr<Expression>> aggregates;
	//! The payload types
	vector<LogicalType> payload_types;
	//! The aggregate return types
	vector<LogicalType> aggregate_return_types;
	//! Pointers to the aggregates
	vector<BoundAggregateExpression *> bindings;
	idx_t filter_count;

public:
	idx_t GroupCount() const;

	const vector<vector<idx_t>> &GetGroupingFunctions() const;

	void InitializeGroupby(vector<unique_ptr<Expression>> groups, vector<unique_ptr<Expression>> expressions,
	                       vector<unsafe_vector<idx_t>> grouping_functions);

	//! Initialize a GroupedAggregateData object for use with distinct aggregates
	void InitializeDistinct(const unique_ptr<Expression> &aggregate, const vector<unique_ptr<Expression>> *groups_p);

private:
	void InitializeDistinctGroups(const vector<unique_ptr<Expression>> *groups);
	void InitializeGroupbyGroups(vector<unique_ptr<Expression>> groups);
	void SetGroupingFunctions(vector<unsafe_vector<idx_t>> &functions);
};

} // namespace goose
