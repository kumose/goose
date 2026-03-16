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

#include <goose/common/constants.h>
#include <goose/common/enums/joinref_type.h>
#include <goose/common/types-import.h>
#include <goose/planner/joinside.h>
#include <goose/planner/operator/logical_join.h>
#include <goose/execution/operator/join/join_filter_pushdown.h>

namespace goose {

//! LogicalComparisonJoin represents a join that involves comparisons between the LHS and RHS
class LogicalComparisonJoin : public LogicalJoin {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	explicit LogicalComparisonJoin(JoinType type,
	                               LogicalOperatorType logical_type = LogicalOperatorType::LOGICAL_COMPARISON_JOIN);

	//! The conditions of the join
	vector<JoinCondition> conditions;
	//! Used for duplicate-eliminated MARK joins
	vector<LogicalType> mark_types;
	//! The set of columns that will be duplicate eliminated from the LHS and pushed into the RHS
	vector<unique_ptr<Expression>> duplicate_eliminated_columns;
	//! If this is a DelimJoin, whether it has been flipped to de-duplicating the RHS instead
	bool delim_flipped = false;
	//! (If join_type == MARK) can this comparison join be converted from a mark join to semi
	bool convert_mark_to_semi = true;
	//! Scans where we should push generated filters into (if any)
	unique_ptr<JoinFilterPushdownInfo> filter_pushdown;
	//! Filtering predicate from the ON clause with expressions that don't reference both sides
	unique_ptr<Expression> predicate;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

public:
	static unique_ptr<LogicalOperator> CreateJoin(ClientContext &context, JoinType type, JoinRefType ref_type,
	                                              unique_ptr<LogicalOperator> left_child,
	                                              unique_ptr<LogicalOperator> right_child,
	                                              unique_ptr<Expression> condition);
	static unique_ptr<LogicalOperator> CreateJoin(JoinType type, JoinRefType ref_type,
	                                              unique_ptr<LogicalOperator> left_child,
	                                              unique_ptr<LogicalOperator> right_child,
	                                              vector<JoinCondition> conditions,
	                                              vector<unique_ptr<Expression>> arbitrary_expressions);

	static void ExtractJoinConditions(ClientContext &context, JoinType type, JoinRefType ref_type,
	                                  unique_ptr<LogicalOperator> &left_child, unique_ptr<LogicalOperator> &right_child,
	                                  unique_ptr<Expression> condition, vector<JoinCondition> &conditions,
	                                  vector<unique_ptr<Expression>> &arbitrary_expressions);
	static void ExtractJoinConditions(ClientContext &context, JoinType type, JoinRefType ref_type,
	                                  unique_ptr<LogicalOperator> &left_child, unique_ptr<LogicalOperator> &right_child,
	                                  vector<unique_ptr<Expression>> &expressions, vector<JoinCondition> &conditions,
	                                  vector<unique_ptr<Expression>> &arbitrary_expressions);
	static void ExtractJoinConditions(ClientContext &context, JoinType type, JoinRefType ref_type,
	                                  unique_ptr<LogicalOperator> &left_child, unique_ptr<LogicalOperator> &right_child,
	                                  const unordered_set<idx_t> &left_bindings,
	                                  const unordered_set<idx_t> &right_bindings,
	                                  vector<unique_ptr<Expression>> &expressions, vector<JoinCondition> &conditions,
	                                  vector<unique_ptr<Expression>> &arbitrary_expressions);

	bool HasEquality(idx_t &range_count) const;
};

} // namespace goose
