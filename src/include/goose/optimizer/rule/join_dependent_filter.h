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

namespace goose {

//! Join-dependent filter derivation as proposed in https://homepages.cwi.nl/~boncz/snb-challenge/chokepoints-tpctc.pdf
//! This rule inspects filters like this:
//!     SELECT *
//!     FROM nation n1
//!     JOIN nation n2
//!     ON ((n1.n_name = 'FRANCE'
//!            AND n2.n_name = 'GERMANY')
//!        OR (n1.n_name = 'GERMANY'
//!            AND n2.n_name = 'FRANCE'));
//! The join filter as a whole cannot be pushed down, because it references tables from both sides.
//! However, we can derive from this two filters that can be pushed down, namely:
//!     WHERE (n1.n_name = 'FRANCE' OR n1.n_name = 'GERMANY')
//!      AND (n2.n_name = 'GERMANY' OR n2.n_name = 'FRANCE')
//! By adding this filter, we can reduce both sides of the join before performing the join on the original condition.
class JoinDependentFilterRule : public Rule {
public:
	explicit JoinDependentFilterRule(ExpressionRewriter &rewriter);

	unique_ptr<Expression> Apply(LogicalOperator &op, vector<reference<Expression>> &bindings, bool &changes_made,
	                             bool is_root) override;
};

} // namespace goose
