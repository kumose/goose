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

#include <goose/optimizer/matcher/expression_matcher.h>
#include <goose/optimizer/matcher/logical_operator_matcher.h>

namespace goose {
class ExpressionRewriter;

class Rule {
public:
	explicit Rule(ExpressionRewriter &rewriter) : rewriter(rewriter) {
	}
	virtual ~Rule() {
	}

	//! The expression rewriter this rule belongs to
	ExpressionRewriter &rewriter;
	//! The expression matcher of the rule
	unique_ptr<ExpressionMatcher> root;

	ClientContext &GetContext() const;
	virtual unique_ptr<Expression> Apply(LogicalOperator &op, vector<reference<Expression>> &bindings,
	                                     bool &fixed_point, bool is_root) = 0;
};

} // namespace goose
