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

#include <goose/common/common.h>
#include <goose/planner/expression.h>

#include <functional>

namespace goose {

class ExpressionIterator {
public:
	static void EnumerateChildren(const Expression &expression,
	                              const std::function<void(const Expression &child)> &callback);
	static void EnumerateChildren(Expression &expression, const std::function<void(Expression &child)> &callback);
	static void EnumerateChildren(Expression &expression,
	                              const std::function<void(unique_ptr<Expression> &child)> &callback);

	static void EnumerateExpression(unique_ptr<Expression> &expr,
	                                const std::function<void(Expression &child)> &callback);
	static void EnumerateExpression(unique_ptr<Expression> &expr,
	                                const std::function<void(unique_ptr<Expression> &child)> &callback);

	static void VisitExpressionClass(const Expression &expr, ExpressionClass expr_class,
	                                 const std::function<void(const Expression &child)> &callback);
	static void VisitExpressionClassMutable(unique_ptr<Expression> &expr, ExpressionClass expr_class,
	                                        const std::function<void(unique_ptr<Expression> &child)> &callback);

	template <class T>
	static void VisitExpressionMutable(unique_ptr<Expression> &expr,
	                                   const std::function<void(T &child, unique_ptr<Expression> &)> &callback) {
		VisitExpressionClassMutable(expr, T::TYPE,
		                            [&](unique_ptr<Expression> &child) { callback(child->Cast<T>(), child); });
	}
	template <class T>
	static void VisitExpression(const Expression &expr, const std::function<void(const T &child)> &callback) {
		VisitExpressionClass(expr, T::TYPE, [&](const Expression &child) { callback(child.Cast<T>()); });
	}
};

} // namespace goose
