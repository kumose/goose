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
#include <goose/common/types-import.h>
#include <goose/optimizer/join_order/join_relation.h>
#include <goose/optimizer/join_order/cardinality_estimator.h>
#include <goose/optimizer/join_order/query_graph.h>
#include <goose/optimizer/join_order/join_node.h>
#include <goose/optimizer/join_order/cost_model.h>
#include <goose/parser/expression_map.h>
#include <goose/common/reference_map.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/logical_operator_visitor.h>

#include <functional>

namespace goose {

class QueryGraphManager;

class PlanEnumerator {
public:
	explicit PlanEnumerator(QueryGraphManager &query_graph_manager, CostModel &cost_model,
	                        const QueryGraphEdges &query_graph)
	    : query_graph(query_graph), query_graph_manager(query_graph_manager), cost_model(cost_model) {
	}

	static constexpr idx_t THRESHOLD_TO_SWAP_TO_APPROXIMATE = 12;

	//! Perform the join order solving
	void SolveJoinOrder();
	void InitLeafPlans();

	const reference_map_t<JoinRelationSet, unique_ptr<DPJoinNode>> &GetPlans() const;

private:
	//! The set of edges used in the join optimizer
	QueryGraphEdges const &query_graph;
	//! The total amount of join pairs that have been considered
	idx_t pairs = 0;
	//! Grant access to the set manager and the relation manager
	QueryGraphManager &query_graph_manager;
	//! Cost model to evaluate cost of joins
	CostModel &cost_model;
	//! A map to store the optimal join plan found for a specific JoinRelationSet*
	reference_map_t<JoinRelationSet, unique_ptr<DPJoinNode>> plans;

	unordered_set<string> join_nodes_in_full_plan;

	unique_ptr<DPJoinNode> CreateJoinTree(JoinRelationSet &set,
	                                      const vector<reference<NeighborInfo>> &possible_connections, DPJoinNode &left,
	                                      DPJoinNode &right);

	//! Emit a pair as a potential join candidate. Returns the best plan found for the (left, right) connection (either
	//! the newly created plan, or an existing plan)
	DPJoinNode &EmitPair(JoinRelationSet &left, JoinRelationSet &right, const vector<reference<NeighborInfo>> &info);
	//! Tries to emit a potential join candidate pair. Returns false if too many pairs have already been emitted,
	//! cancelling the dynamic programming step.
	bool TryEmitPair(JoinRelationSet &left, JoinRelationSet &right, const vector<reference<NeighborInfo>> &info);

	bool EnumerateCmpRecursive(JoinRelationSet &left, JoinRelationSet &right, unordered_set<idx_t> &exclusion_set);
	//! Emit a relation set node
	bool EmitCSG(JoinRelationSet &node);
	//! Enumerate the possible connected subgraphs that can be joined together in the join graph
	bool EnumerateCSGRecursive(JoinRelationSet &node, unordered_set<idx_t> &exclusion_set);
	//! Generate cross product edges inside the side
	void GenerateCrossProducts();

	//! Solve the join order exactly using dynamic programming. Returns true if it was completed successfully (i.e. did
	//! not time-out)
	bool SolveJoinOrderExactly();
	//! Solve the join order approximately using a greedy algorithm
	void SolveJoinOrderApproximately();
};

} // namespace goose
