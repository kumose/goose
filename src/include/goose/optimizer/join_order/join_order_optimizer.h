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
#include <goose/optimizer/join_order/cardinality_estimator.h>
#include <goose/optimizer/join_order/join_node.h>
#include <goose/optimizer/join_order/join_relation.h>
#include <goose/optimizer/join_order/query_graph.h>
#include <goose/optimizer/join_order/query_graph_manager.h>
#include <goose/parser/expression_map.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/logical_operator_visitor.h>

#include <functional>

namespace goose {

class JoinOrderOptimizer {
public:
	explicit JoinOrderOptimizer(ClientContext &context);
	JoinOrderOptimizer CreateChildOptimizer();

public:
	//! Perform join reordering inside a plan
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> plan, optional_ptr<RelationStats> stats = nullptr);
	//! Adds/gets materialized CTE stats
	void AddMaterializedCTEStats(idx_t index, RelationStats &&stats);
	RelationStats GetMaterializedCTEStats(idx_t index);
	//! Adds/gets delim scan stats
	void AddDelimScanStats(RelationStats &stats);
	RelationStats GetDelimScanStats();

private:
	ClientContext &context;

	//! manages the query graph, relations, and edges between relations
	QueryGraphManager query_graph_manager;

	//! The set of filters extracted from the query graph
	vector<unique_ptr<Expression>> filters;
	//! The set of filter infos created from the extracted filters
	vector<unique_ptr<FilterInfo>> filter_infos;
	//! A map of all expressions a given expression has to be equivalent to. This is used to add "implied join edges".
	//! i.e. in the join A=B AND B=C, the equivalence set of {B} is {A, C}, thus we can add an implied join edge {A = C}
	expression_map_t<vector<FilterInfo *>> equivalence_sets;

	CardinalityEstimator cardinality_estimator;

	unordered_set<std::string> join_nodes_in_full_plan;

	//! Mapping from materialized CTE index to stats
	unordered_map<idx_t, RelationStats> materialized_cte_stats;
	//! Stats of Delim Scans of the Delim Join that is currently being optimized
	optional_ptr<RelationStats> delim_scan_stats;
	idx_t depth;

public:
	unordered_set<idx_t> recursive_cte_indexes;
};

} // namespace goose
