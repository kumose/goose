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
#include <goose/parser/expression_map.h>
#include <goose/planner/expression_binder.h>

namespace goose {
class BoundColumnRefExpression;
class WindowExpression;

class BoundSelectNode;

struct BoundGroupInformation {
	parsed_expression_map_t<idx_t> map;
	case_insensitive_map_t<idx_t> alias_map;
	unordered_map<idx_t, idx_t> collated_groups;
};

//! The BaseSelectBinder is the base binder of the SELECT, HAVING and QUALIFY binders. It can bind aggregates and window
//! functions.
class BaseSelectBinder : public ExpressionBinder {
public:
	BaseSelectBinder(Binder &binder, ClientContext &context, BoundSelectNode &node, BoundGroupInformation &info);

	bool BoundAggregates() {
		return bound_aggregate;
	}
	void ResetBindings() {
		this->bound_aggregate = false;
		this->bound_columns.clear();
	}

protected:
	BindResult BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth,
	                          bool root_expression = false) override;

	BindResult BindAggregate(FunctionExpression &expr, AggregateFunctionCatalogEntry &function, idx_t depth) override;

	bool inside_window;
	bool bound_aggregate = false;

	BoundSelectNode &node;
	BoundGroupInformation &info;

protected:
	BindResult BindGroupingFunction(OperatorExpression &op, idx_t depth) override;

	//! Binds a WINDOW expression and returns the result.
	virtual BindResult BindWindow(WindowExpression &expr, idx_t depth);
	virtual BindResult BindColumnRef(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression);

	idx_t TryBindGroup(ParsedExpression &expr);
	BindResult BindGroup(ParsedExpression &expr, idx_t depth, idx_t group_index);
};

} // namespace goose
