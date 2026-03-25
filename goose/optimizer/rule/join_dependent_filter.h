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
