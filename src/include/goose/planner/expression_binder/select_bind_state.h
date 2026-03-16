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

#include <goose/planner/bound_query_node.h>
#include <goose/planner/logical_operator.h>
#include <goose/parser/expression_map.h>
#include <goose/parser/parsed_data/sample_options.h>
#include <goose/parser/group_by_node.h>

namespace goose {

//! Bind state during a SelectNode
struct SelectBindState {
	// Mapping of (alias -> index) and a mapping of (Expression -> index) for the SELECT list
	case_insensitive_map_t<idx_t> alias_map;
	parsed_expression_map_t<idx_t> projection_map;
	//! The original unparsed expressions. This is exported after binding, because the binding might change the
	//! expressions (e.g. when a * clause is present)
	vector<unique_ptr<ParsedExpression>> original_expressions;

public:
	unique_ptr<ParsedExpression> BindAlias(idx_t index);

	void SetExpressionIsVolatile(idx_t index);
	void SetExpressionHasSubquery(idx_t index);

	bool AliasHasSubquery(idx_t index) const;

	void AddExpandedColumn(idx_t expand_count);
	void AddRegularColumn();
	idx_t GetFinalIndex(idx_t index) const;

private:
	//! The set of referenced aliases
	unordered_set<idx_t> referenced_aliases;
	//! The set of expressions that is volatile
	unordered_set<idx_t> volatile_expressions;
	//! The set of expressions that contains a subquery
	unordered_set<idx_t> subquery_expressions;
	//! Column indices after expansion of Expanded expressions (e.g. UNNEST(STRUCT) clauses)
	vector<idx_t> expanded_column_indices;
};

} // namespace goose
