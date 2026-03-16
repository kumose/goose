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

#include <goose/common/case_insensitive_map.h>
#include <goose/planner/expression_binder.h>

namespace goose {
class ConstantExpression;
class ColumnRefExpression;
struct SelectBindState;

//! The GROUP binder is responsible for binding expressions in the GROUP BY clause
class GroupBinder : public ExpressionBinder {
public:
	GroupBinder(Binder &binder, ClientContext &context, SelectNode &node, idx_t group_index,
	            SelectBindState &bind_state, case_insensitive_map_t<idx_t> &group_alias_map);

	//! The unbound root expression
	unique_ptr<ParsedExpression> unbound_expression;
	//! The group index currently being bound
	idx_t bind_index;

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression) override;

	string UnsupportedAggregateMessage() override;

	BindResult BindSelectRef(idx_t entry);
	BindResult BindColumnRef(ColumnRefExpression &expr, unique_ptr<ParsedExpression> &expr_ptr);
	BindResult BindConstant(ConstantExpression &expr);

	bool TryResolveAliasReference(ColumnRefExpression &colref, idx_t depth, bool root_expression, BindResult &result,
	                              unique_ptr<ParsedExpression> &expr_ptr) override;

	SelectNode &node;
	SelectBindState &bind_state;
	case_insensitive_map_t<idx_t> &group_alias_map;
	unordered_set<idx_t> used_aliases;

	idx_t group_index;
};

} // namespace goose
