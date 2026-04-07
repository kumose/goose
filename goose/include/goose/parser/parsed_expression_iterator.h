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

#include <goose/parser/parsed_expression.h>
#include <goose/parser/tokens.h>

#include <functional>

namespace goose {

class ParsedExpressionIterator {
public:
	static void EnumerateChildren(const ParsedExpression &expression,
	                              const std::function<void(const ParsedExpression &child)> &callback);
	static void EnumerateChildren(ParsedExpression &expr, const std::function<void(ParsedExpression &child)> &callback);
	static void EnumerateChildren(ParsedExpression &expr,
	                              const std::function<void(unique_ptr<ParsedExpression> &child)> &callback);

	static void EnumerateTableRefChildren(TableRef &ref,
	                                      const std::function<void(unique_ptr<ParsedExpression> &child)> &expr_callback,
	                                      const std::function<void(TableRef &ref)> &ref_callback = DefaultRefCallback);
	static void
	EnumerateQueryNodeChildren(QueryNode &node,
	                           const std::function<void(unique_ptr<ParsedExpression> &child)> &expr_callback,
	                           const std::function<void(TableRef &ref)> &ref_callback = DefaultRefCallback);

	static void
	EnumerateQueryNodeModifiers(QueryNode &node,
	                            const std::function<void(unique_ptr<ParsedExpression> &child)> &expr_callback);

	static void VisitExpressionClass(const ParsedExpression &expr, ExpressionClass expr_class,
	                                 const std::function<void(const ParsedExpression &child)> &callback);
	static void VisitExpressionClassMutable(ParsedExpression &expr, ExpressionClass expr_class,
	                                        const std::function<void(ParsedExpression &child)> &callback);

	template <class T>
	static void VisitExpressionMutable(ParsedExpression &expr, const std::function<void(T &child)> &callback) {
		VisitExpressionClassMutable(expr, T::TYPE, [&](ParsedExpression &child) { callback(child.Cast<T>()); });
	}
	template <class T>
	static void VisitExpression(const ParsedExpression &expr, const std::function<void(const T &child)> &callback) {
		VisitExpressionClass(expr, T::TYPE, [&](const ParsedExpression &child) { callback(child.Cast<T>()); });
	}

private:
	static void DefaultRefCallback(TableRef &ref) {}; // NOP
};

} // namespace goose
