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

#include <goose/optimizer/join_order/join_relation.h>
#include <goose/optimizer/join_order/query_graph.h>

namespace goose {

struct NeighborInfo;

class DPJoinNode {
public:
	//! Represents a node in the join plan
	JoinRelationSet &set;
	//! information on how left and right are connected
	optional_ptr<NeighborInfo> info;
	bool is_leaf;
	//! left and right plans
	JoinRelationSet &left_set;
	JoinRelationSet &right_set;

	//! The cost of the join node. The cost is stored here so that the cost of
	//! a join node stays in sync with how the join node is constructed. Storing the cost in an unordered_set
	//! in the cost model is error prone. If the plan enumerator join node is updated and not the cost model
	//! the whole Join Order Optimizer can start exhibiting undesired behavior.
	double cost;
	//! used only to populate logical operators with estimated cardinalities after the best join plan has been found.
	idx_t cardinality;

	//! Create an intermediate node in the join tree. base_cardinality = estimated_props.cardinality
	DPJoinNode(JoinRelationSet &set, optional_ptr<NeighborInfo> info, JoinRelationSet &left, JoinRelationSet &right,
	           double cost);

	//! Create a leaf node in the join tree
	//! set cost to 0 for leaf nodes
	//! cost will be the cost to *produce* an intermediate table
	explicit DPJoinNode(JoinRelationSet &set);
};

} // namespace goose
