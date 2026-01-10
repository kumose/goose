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
