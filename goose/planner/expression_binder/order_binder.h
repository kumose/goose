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
