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
#include <goose/common/optional_ptr.h>
#include <goose/optimizer/join_order/join_relation.h>
#include <goose/optimizer/join_order/join_node.h>
#include <goose/optimizer/join_order/relation_manager.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/vector.h>
#include <goose/planner/column_binding.h>

#include <functional>

namespace goose {

class FilterInfo;

struct NeighborInfo {
	explicit NeighborInfo(optional_ptr<JoinRelationSet> neighbor) : neighbor(neighbor) {
	}

	optional_ptr<JoinRelationSet> neighbor;
	vector<optional_ptr<FilterInfo>> filters;
};

//! The QueryGraph contains edges between relations and allows edges to be created/queried
class QueryGraphEdges {
public:
	//! Contains a node with info about neighboring relations and child edge infos
	struct QueryEdge {
		vector<unique_ptr<NeighborInfo>> neighbors;
		unordered_map<idx_t, unique_ptr<QueryEdge>> children;
	};

public:
	string ToString() const;
	void Print();

	//! Returns a connection if there is an edge that connects these two sets, or nullptr otherwise
	const vector<reference<NeighborInfo>> GetConnections(JoinRelationSet &node, JoinRelationSet &other) const;
	//! Enumerate the neighbors of a specific node that do not belong to any of the exclusion_set. Note that if a
	//! neighbor has multiple nodes, this function will return the lowest entry in that set.
	const vector<idx_t> GetNeighbors(JoinRelationSet &node, unordered_set<idx_t> &exclusion_set) const;

	//! Enumerate all neighbors of a given JoinRelationSet node
	void EnumerateNeighbors(JoinRelationSet &node, const std::function<bool(NeighborInfo &)> &callback) const;
	//! Create an edge in the edge_set
	void CreateEdge(JoinRelationSet &left, JoinRelationSet &right, optional_ptr<FilterInfo> info);

private:
	//! Get the QueryEdge of a specific node
	optional_ptr<QueryEdge> GetQueryEdge(JoinRelationSet &left);

	void EnumerateNeighborsDFS(JoinRelationSet &node, reference<QueryEdge> info, idx_t index,
	                           const std::function<bool(NeighborInfo &)> &callback) const;

	QueryEdge root;
};

} // namespace goose
