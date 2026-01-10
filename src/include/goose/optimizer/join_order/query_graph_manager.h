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

#include <goose/common/common.h>
#include <goose/common/enums/join_type.h>
#include <goose/common/optional_ptr.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/vector.h>
#include <goose/optimizer/join_order/join_node.h>
#include <goose/optimizer/join_order/join_relation.h>
#include <goose/optimizer/join_order/query_graph.h>
#include <goose/optimizer/join_order/relation_manager.h>
#include <goose/planner/column_binding.h>
#include <goose/planner/logical_operator.h>

#include <functional>

namespace goose {

class QueryGraphEdges;

struct GenerateJoinRelation {
	GenerateJoinRelation(optional_ptr<JoinRelationSet> set, unique_ptr<LogicalOperator> op_p)
	    : set(set), op(std::move(op_p)) {
	}

	optional_ptr<JoinRelationSet> set;
	unique_ptr<LogicalOperator> op;
};

//! Filter info struct that is used by the cardinality estimator to set the initial cardinality
//! but is also eventually transformed into a query edge.
class FilterInfo {
public:
	FilterInfo(unique_ptr<Expression> filter, JoinRelationSet &set, idx_t filter_index,
	           JoinType join_type = JoinType::INNER)
	    : filter(std::move(filter)), set(set), filter_index(filter_index), join_type(join_type) {
	}

public:
	unique_ptr<Expression> filter;
	reference<JoinRelationSet> set;
	idx_t filter_index;
	JoinType join_type;
	optional_ptr<JoinRelationSet> left_set;
	optional_ptr<JoinRelationSet> right_set;
	ColumnBinding left_binding;
	ColumnBinding right_binding;

	void SetLeftSet(optional_ptr<JoinRelationSet> left_set_new);
	void SetRightSet(optional_ptr<JoinRelationSet> right_set_new);
};

//! The QueryGraphManager manages the process of extracting the reorderable and nonreorderable operations
//! from the logical plan and creating the intermediate structures needed by the plan enumerator.
//! When the plan enumerator finishes, the Query Graph Manger can then recreate the logical plan.
class QueryGraphManager {
public:
	explicit QueryGraphManager(ClientContext &context) : relation_manager(context), context(context) {
	}

	//! manage relations and the logical operators they represent
	RelationManager relation_manager;

	//! A structure holding all the created JoinRelationSet objects
	JoinRelationSetManager set_manager;

	ClientContext &context;

	//! Extract the join relations, optimizing non-reoderable relations when encountered
	bool Build(JoinOrderOptimizer &optimizer, LogicalOperator &op);

	//! Reconstruct the logical plan using the plan found by the plan enumerator
	unique_ptr<LogicalOperator> Reconstruct(unique_ptr<LogicalOperator> plan);

	//! Get a reference to the QueryGraphEdges structure that stores edges between
	//! nodes and hypernodes.
	const QueryGraphEdges &GetQueryGraphEdges() const;

	//! Get a list of the join filters in the join plan than eventually are
	//! transformed into the query graph edges
	const vector<unique_ptr<FilterInfo>> &GetFilterBindings() const;

	//! Plan enumerator may not find a full plan and therefore will need to create cross
	//! products to create edges.
	void CreateQueryGraphCrossProduct(JoinRelationSet &left, JoinRelationSet &right);

	//! A map to store the optimal join plan found for a specific JoinRelationSet*
	optional_ptr<const reference_map_t<JoinRelationSet, unique_ptr<DPJoinNode>>> plans;

private:
	vector<reference<LogicalOperator>> filter_operators;

	//! Filter information including the column_bindings that join filters
	//! used by the cardinality estimator to estimate distinct counts
	vector<unique_ptr<FilterInfo>> filters_and_bindings;

	QueryGraphEdges query_graph;

	void GetColumnBinding(Expression &expression, ColumnBinding &binding);

	void CreateHyperGraphEdges();

	GenerateJoinRelation GenerateJoins(vector<unique_ptr<LogicalOperator>> &extracted_relations, JoinRelationSet &set);
};

} // namespace goose
