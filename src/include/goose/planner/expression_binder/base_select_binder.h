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
