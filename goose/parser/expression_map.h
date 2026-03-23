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

#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/parser/parsed_expression.h>
#include <goose/planner/expression.h>

namespace goose {
class Expression;

template <class T>
struct ExpressionHashFunction {
	uint64_t operator()(const reference<T> &expr) const {
		return (uint64_t)expr.get().Hash();
	}
};

template <class T>
struct ExpressionEquality {
	bool operator()(const reference<T> &a, const reference<T> &b) const {
		return a.get().Equals(b.get());
	}
};

template <typename T>
using expression_map_t =
    unordered_map<reference<Expression>, T, ExpressionHashFunction<Expression>, ExpressionEquality<Expression>>;

using expression_set_t =
    unordered_set<reference<Expression>, ExpressionHashFunction<Expression>, ExpressionEquality<Expression>>;

template <typename T>
using parsed_expression_map_t = unordered_map<reference<ParsedExpression>, T, ExpressionHashFunction<ParsedExpression>,
                                              ExpressionEquality<ParsedExpression>>;

using parsed_expression_set_t = unordered_set<reference<ParsedExpression>, ExpressionHashFunction<ParsedExpression>,
                                              ExpressionEquality<ParsedExpression>>;

} // namespace goose
