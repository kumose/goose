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
