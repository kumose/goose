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

#include <goose/planner/bound_query_node.h>
#include <goose/planner/logical_operator.h>
#include <goose/parser/expression_map.h>
#include <goose/parser/parsed_data/sample_options.h>
#include <goose/parser/group_by_node.h>
#include <goose/planner/expression_binder/select_bind_state.h>

namespace goose {

class BoundGroupByNode {
public:
	//! The total set of all group expressions
	vector<unique_ptr<Expression>> group_expressions;
	//! The different grouping sets as they map to the group expressions
	vector<GroupingSet> grouping_sets;
};

struct BoundUnnestNode {
	//! The index of the UNNEST node
	idx_t index;
	//! The set of expressions
	vector<unique_ptr<Expression>> expressions;
};

//! Bound equivalent of SelectNode
class BoundSelectNode : public BoundQueryNode {
public:
	//! Bind information
	SelectBindState bind_state;
	//! The projection list
	vector<unique_ptr<Expression>> select_list;
	//! The FROM clause
	BoundStatement from_table;
	//! The WHERE clause
	unique_ptr<Expression> where_clause;
	//! list of groups
	BoundGroupByNode groups;
	//! HAVING clause
	unique_ptr<Expression> having;
	//! QUALIFY clause
	unique_ptr<Expression> qualify;
	//! SAMPLE clause
	unique_ptr<SampleOptions> sample_options;

	//! The amount of columns in the final result
	idx_t column_count;
	//! The amount of bound columns in the select list
	idx_t bound_column_count = 0;

	//! Index used by the LogicalProjection
	idx_t projection_index;

	//! Group index used by the LogicalAggregate (only used if HasAggregation is true)
	idx_t group_index;
	//! Table index for the projection child of the group op
	idx_t group_projection_index;
	//! Aggregate index used by the LogicalAggregate (only used if HasAggregation is true)
	idx_t aggregate_index;
	//! Index used for GROUPINGS column references
	idx_t groupings_index;
	//! Aggregate functions to compute (only used if HasAggregation is true)
	vector<unique_ptr<Expression>> aggregates;

	//! GROUPING function calls
	vector<unsafe_vector<idx_t>> grouping_functions;

	//! Map from aggregate function to aggregate index (used to eliminate duplicate aggregates)
	expression_map_t<idx_t> aggregate_map;

	//! Window index used by the LogicalWindow (only used if HasWindow is true)
	idx_t window_index;
	//! Window functions to compute (only used if HasWindow is true)
	vector<unique_ptr<Expression>> windows;

	//! Unnest expression
	unordered_map<idx_t, BoundUnnestNode> unnests;

	//! Index of pruned node
	idx_t prune_index;
	bool need_prune = false;

public:
	idx_t GetRootIndex() override {
		return need_prune ? prune_index : projection_index;
	}
};
} // namespace goose
