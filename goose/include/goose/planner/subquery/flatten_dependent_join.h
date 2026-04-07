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
#include <goose/planner/binder.h>
#include <goose/planner/column_binding_map.h>
#include <goose/planner/logical_operator.h>

namespace goose {

//! The FlattenDependentJoins class is responsible for pushing the dependent join down into the plan to create a
//! flattened subquery
struct FlattenDependentJoins {
	FlattenDependentJoins(Binder &binder, const CorrelatedColumns &correlated, bool perform_delim = true,
	                      bool any_join = false, optional_ptr<FlattenDependentJoins> parent = nullptr);

	static unique_ptr<LogicalOperator> DecorrelateIndependent(Binder &binder, unique_ptr<LogicalOperator> plan);

	unique_ptr<LogicalOperator> Decorrelate(unique_ptr<LogicalOperator> plan, bool parent_propagate_null_values = true,
	                                        idx_t lateral_depth = 0);

private:
	//! Detects which Logical Operators have correlated expressions that they are dependent upon, filling the
	//! has_correlated_expressions map.
	bool DetectCorrelatedExpressions(LogicalOperator &op, bool lateral = false, idx_t lateral_depth = 0,
	                                 bool parent_is_dependent_join = false);

	//! Mark entire subtree of Logical Operators as correlated by adding them to the has_correlated_expressions map.
	bool MarkSubtreeCorrelated(LogicalOperator &op, idx_t cte_index);

	//! Push the dependent join down a LogicalOperator
	unique_ptr<LogicalOperator> PushDownDependentJoin(unique_ptr<LogicalOperator> plan,
	                                                  bool propagates_null_values = true, idx_t lateral_depth = 0);

public:
	Binder &binder;
	ColumnBinding base_binding;
	idx_t delim_offset;
	idx_t data_offset;
	reference_map_t<LogicalOperator, bool> has_correlated_expressions;
	column_binding_map_t<idx_t> correlated_map;
	column_binding_map_t<idx_t> replacement_map;
	const CorrelatedColumns &correlated_columns;
	vector<LogicalType> delim_types;

	bool perform_delim;
	bool any_join;
	optional_ptr<FlattenDependentJoins> parent;

private:
	unique_ptr<LogicalOperator> PushDownDependentJoinInternal(unique_ptr<LogicalOperator> plan,
	                                                          bool &parent_propagate_null_values, idx_t lateral_depth);
};

} // namespace goose
