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

class ColumnRefExpression;
struct SelectBindState;

//! A helper binder for WhereBinder and HavingBinder which support alias as a columnref.
class ColumnAliasBinder {
public:
	explicit ColumnAliasBinder(SelectBindState &bind_state);

	bool BindAlias(ExpressionBinder &enclosing_binder, unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	               bool root_expression, BindResult &result);
	// Check if the column reference is an SELECT item alias.
	bool DoesColumnAliasExist(const ColumnRefExpression &colref);

private:
	SelectBindState &bind_state;
	unordered_set<idx_t> visited_select_indexes;
};

} // namespace goose
