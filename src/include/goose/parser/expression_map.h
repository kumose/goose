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
