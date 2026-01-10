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
#include <goose/parser/parsed_expression.h>
#include <goose/planner/expression/bound_columnref_expression.h>

namespace goose {
class Binder;
class Expression;
class SelectNode;
struct SelectBindState;

//! The ORDER binder is responsible for binding an expression within the ORDER BY clause of a SQL statement
class OrderBinder {
public:
	OrderBinder(vector<reference<Binder>> binders, SelectBindState &bind_state);
	OrderBinder(vector<reference<Binder>> binders, SelectNode &node, SelectBindState &bind_state);

public:
	unique_ptr<Expression> Bind(unique_ptr<ParsedExpression> expr);

	bool HasExtraList() const {
		return extra_list;
	}
	const vector<reference<Binder>> &GetBinders() const {
		return binders;
	}

	unique_ptr<Expression> CreateExtraReference(unique_ptr<ParsedExpression> expr);

	//! Sets the query component, for error messages
	void SetQueryComponent(string component = string());

private:
	unique_ptr<Expression> CreateProjectionReference(ParsedExpression &expr, const idx_t index);
	unique_ptr<Expression> BindConstant(ParsedExpression &expr);
	optional_idx TryGetProjectionReference(ParsedExpression &expr) const;

private:
	vector<reference<Binder>> binders;
	optional_ptr<vector<unique_ptr<ParsedExpression>>> extra_list;
	SelectBindState &bind_state;
	string query_component = "ORDER BY";
};

} // namespace goose
