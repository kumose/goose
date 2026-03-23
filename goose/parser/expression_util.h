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

#include <goose/parser/base_expression.h>
#include <goose/common/vector.h>

namespace goose {
class ParsedExpression;
class Expression;

class ExpressionUtil {
public:
	//! ListEquals: check if a list of two expressions is equal (order is important)
	static bool ListEquals(const vector<unique_ptr<ParsedExpression>> &a,
	                       const vector<unique_ptr<ParsedExpression>> &b);
	static bool ListEquals(const vector<unique_ptr<Expression>> &a, const vector<unique_ptr<Expression>> &b);
	//! SetEquals: check if two sets of expressions are equal (order is not important)
	static bool SetEquals(const vector<unique_ptr<ParsedExpression>> &a, const vector<unique_ptr<ParsedExpression>> &b);
	static bool SetEquals(const vector<unique_ptr<Expression>> &a, const vector<unique_ptr<Expression>> &b);

private:
	template <class T>
	static bool ExpressionListEquals(const vector<unique_ptr<T>> &a, const vector<unique_ptr<T>> &b);
	template <class T, class EXPRESSION_MAP>
	static bool ExpressionSetEquals(const vector<unique_ptr<T>> &a, const vector<unique_ptr<T>> &b);
};

} // namespace goose
