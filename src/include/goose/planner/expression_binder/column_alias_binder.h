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
