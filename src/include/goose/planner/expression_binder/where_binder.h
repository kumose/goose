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

#include <goose/planner/expression_binder.h>

namespace goose {

class ColumnAliasBinder;

//! The WHERE binder is responsible for binding an expression within the WHERE clause of a SQL statement
class WhereBinder : public ExpressionBinder {
public:
	WhereBinder(Binder &binder, ClientContext &context, optional_ptr<ColumnAliasBinder> column_alias_binder = nullptr);

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	                          bool root_expression = false) override;

	string UnsupportedAggregateMessage() override;

	bool TryResolveAliasReference(ColumnRefExpression &colref, idx_t depth, bool root_expression, BindResult &result,
	                              unique_ptr<ParsedExpression> &expr_ptr) override;

	bool DoesColumnAliasExist(const ColumnRefExpression &colref) override;

private:
	BindResult BindColumnRef(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression);

	optional_ptr<ColumnAliasBinder> column_alias_binder;
};

} // namespace goose
