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

#include <goose/common/common.h>
#include <goose/planner/bound_tokens.h>
#include <goose/planner/logical_tokens.h>

#include <goose/planner/logical_operator_visitor.h>
#include <goose/planner/binder.h>
#include <goose/planner/bound_parameter_map.h>

namespace goose {

//! While it is possible to copy a logical plan using `Copy()`, both, the original and the copy
//! are—by design—identical. This includes any table_idx values etc. This is bad, when we try
//! to use part of a logical plan multiple times in the same plan.
//! The LogicalOperatorDeepCopy first copies a LogicalOperator, but then traverses the entire plan
//! and replaces all table indexes. We store a map from the original table index to the new index,
//! which we use in the TableBindingReplacer to correct all column accesses.
class LogicalOperatorDeepCopy : public LogicalOperatorVisitor {
public:
	LogicalOperatorDeepCopy(Binder &binder, optional_ptr<bound_parameter_map_t> parameter_data);

	unique_ptr<LogicalOperator> DeepCopy(unique_ptr<LogicalOperator> &op);

private:
	void VisitOperator(LogicalOperator &op) override;

private:
	// Single-field version
	template <typename T>
	void ReplaceTableIndex(LogicalOperator &op);
	// Multi-field version
	template <typename T>
	void ReplaceTableIndexMulti(LogicalOperator &op);

private:
	Binder &binder;
	std::unordered_map<idx_t, idx_t> table_idx_replacements;
	optional_ptr<bound_parameter_map_t> parameter_data;
};

//! The TableBindingReplacer updates table bindings, utility for optimizers
class TableBindingReplacer : LogicalOperatorVisitor {
public:
	TableBindingReplacer(std::unordered_map<idx_t, idx_t> &table_idx_replacements,
	                     optional_ptr<bound_parameter_map_t> parameter_data);

public:
	//! Update each operator of the plan
	void VisitOperator(LogicalOperator &op) override;
	//! Visit an expression and update its column bindings
	void VisitExpression(unique_ptr<Expression> *expression) override;

public:
	//! Contains all bindings that need to be updated
	const std::unordered_map<idx_t, idx_t> &table_idx_replacements;
	optional_ptr<bound_parameter_map_t> parameter_data;
};
} // namespace goose
