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
