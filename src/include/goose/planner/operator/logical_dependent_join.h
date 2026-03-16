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

#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_comparison_join.h>
#include <goose/planner/expression/bound_subquery_expression.h>

namespace goose {

class LogicalDependentJoin : public LogicalComparisonJoin {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DEPENDENT_JOIN;

public:
	explicit LogicalDependentJoin(unique_ptr<LogicalOperator> left, unique_ptr<LogicalOperator> right,
	                              CorrelatedColumns correlated_columns, JoinType type,
	                              unique_ptr<Expression> condition);

	explicit LogicalDependentJoin(JoinType type);

	//! The conditions of the join
	unique_ptr<Expression> join_condition;
	//! The list of columns that have correlations with the right
	CorrelatedColumns correlated_columns;

	SubqueryType subquery_type = SubqueryType::INVALID;
	bool perform_delim = true;
	bool any_join = false;
	bool propagate_null_values = true;
	bool is_lateral_join = false;

	vector<unique_ptr<Expression>> arbitrary_expressions;
	vector<unique_ptr<Expression>> expression_children;
	vector<LogicalType> child_types;
	vector<LogicalType> child_targets;
	ExpressionType comparison_type;

public:
	static unique_ptr<LogicalOperator> Create(unique_ptr<LogicalOperator> left, unique_ptr<LogicalOperator> right,
	                                          CorrelatedColumns correlated_columns, JoinType type,
	                                          unique_ptr<Expression> condition);
};
} // namespace goose
