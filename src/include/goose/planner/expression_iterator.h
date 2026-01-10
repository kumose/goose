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
