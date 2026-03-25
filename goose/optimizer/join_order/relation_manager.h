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
#include <goose/optimizer/join_order/relation_statistics_helper.h>
#include <goose/parser/expression_map.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/logical_operator_visitor.h>

namespace goose {

class JoinOrderOptimizer;
class FilterInfo;

//! Represents a single relation and any metadata accompanying that relation
struct SingleJoinRelation {
	LogicalOperator &op;
	optional_ptr<LogicalOperator> parent;
	RelationStats stats;

	SingleJoinRelation(LogicalOperator &op, optional_ptr<LogicalOperator> parent) : op(op), parent(parent) {
	}
	SingleJoinRelation(LogicalOperator &op, optional_ptr<LogicalOperator> parent, RelationStats stats)
	    : op(op), parent(parent), stats(std::move(stats)) {
	}
};

class RelationManager {
public:
	explicit RelationManager(ClientContext &context) : context(context) {
	}

	idx_t NumRelations();

	bool ExtractJoinRelations(JoinOrderOptimizer &optimizer, LogicalOperator &input_op,
	                          vector<reference<LogicalOperator>> &filter_operators,
	                          optional_ptr<LogicalOperator> parent = nullptr);

	//! for each join filter in the logical plan op, extract the relations that are referred to on
	//! both sides of the join filter, along with the tables & indexes.
	vector<unique_ptr<FilterInfo>> ExtractEdges(LogicalOperator &op,
	                                            vector<reference<LogicalOperator>> &filter_operators,
	                                            JoinRelationSetManager &set_manager);

	//! Extract the set of relations referred to inside an expression
	bool ExtractBindings(Expression &expression, unordered_set<idx_t> &bindings);
	void AddRelation(LogicalOperator &op, optional_ptr<LogicalOperator> parent, const RelationStats &stats);
	//! Add an unnest relation which can come from a logical unnest or a logical get which has an unnest function
	void AddRelationWithChildren(JoinOrderOptimizer &optimizer, LogicalOperator &op, LogicalOperator &input_op,
	                             optional_ptr<LogicalOperator> parent, RelationStats &child_stats,
	                             optional_ptr<LogicalOperator> limit_op,
	                             vector<reference<LogicalOperator>> &datasource_filters);
	void AddAggregateOrWindowRelation(LogicalOperator &op, optional_ptr<LogicalOperator> parent,
	                                  const RelationStats &stats, LogicalOperatorType op_type);
	vector<unique_ptr<SingleJoinRelation>> GetRelations();

	const vector<RelationStats> GetRelationStats();
	//! A mapping of base table index -> index into relations array (relation number)
	unordered_map<idx_t, idx_t> relation_mapping;

	bool CrossProductWithRelationAllowed(idx_t relation_id);

	void PrintRelationStats();

private:
	ClientContext &context;
	//! Set of all relations considered in the join optimizer
	vector<unique_ptr<SingleJoinRelation>> relations;
	unordered_set<idx_t> no_cross_product_relations;
};

} // namespace goose
