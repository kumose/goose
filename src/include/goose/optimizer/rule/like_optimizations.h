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

#include <goose/optimizer/rule.h>
#include <goose/function/scalar/string_functions.h>

namespace goose {

// The Like Optimization rule rewrites LIKE to optimized scalar functions (e.g.: prefix, suffix, and contains)
class LikeOptimizationRule : public Rule {
public:
	explicit LikeOptimizationRule(ExpressionRewriter &rewriter);

	unique_ptr<Expression> Apply(LogicalOperator &op, vector<reference<Expression>> &bindings, bool &changes_made,
	                             bool is_root) override;

	unique_ptr<Expression> ApplyRule(BoundFunctionExpression &expr, ScalarFunction function, string pattern,
	                                 bool is_not_like);
};

} // namespace goose
