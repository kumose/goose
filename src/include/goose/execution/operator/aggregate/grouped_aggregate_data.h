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
