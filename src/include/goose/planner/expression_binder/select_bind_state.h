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
