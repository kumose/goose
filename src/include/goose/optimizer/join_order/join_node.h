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
