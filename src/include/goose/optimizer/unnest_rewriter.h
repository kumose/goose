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

#include <goose/common/types-import.h>
#include <goose/planner/logical_operator.h>

namespace goose {

class Optimizer;

struct ReplaceBinding {
	ReplaceBinding() {};
	ReplaceBinding(ColumnBinding old_binding, ColumnBinding new_binding)
	    : old_binding(old_binding), new_binding(new_binding) {
	}
	ColumnBinding old_binding;
	ColumnBinding new_binding;
};

struct LHSBinding {
	LHSBinding() {};
	LHSBinding(ColumnBinding binding, LogicalType type_p) : binding(binding), type(std::move(type_p)) {
	}
	ColumnBinding binding;
	LogicalType type;
	string alias;
};

//! The UnnestRewriterPlanUpdater updates column bindings after changing the operator plan
class UnnestRewriterPlanUpdater : LogicalOperatorVisitor {
public:
	UnnestRewriterPlanUpdater() {
	}
	//! Update each operator of the plan after moving an UNNEST into a projection
	void VisitOperator(LogicalOperator &op) override;
	//! Visit an expression and update its column bindings after moving and UNNEST into a projection
	void VisitExpression(unique_ptr<Expression> *expression) override;

	//! Contains all bindings that need to be updated
	vector<ReplaceBinding> replace_bindings;
	//! Stores the table index of the former child of the LOGICAL_UNNEST
	idx_t overwritten_tbl_idx;
};

//! The UnnestRewriter optimizer traverses the logical operator tree and rewrites duplicate
//! eliminated joins that contain UNNESTs by moving the UNNESTs into the projection of
//! the SELECT
class UnnestRewriter {
public:
	UnnestRewriter() {
	}
	//! Rewrite duplicate eliminated joins with UNNESTs
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);

private:
	//! Find delim joins that contain an UNNEST
	void FindCandidates(unique_ptr<LogicalOperator> &root, unique_ptr<LogicalOperator> &op,
	                    vector<reference<unique_ptr<LogicalOperator>>> &candidates);
	//! Rewrite a delim join that contains an UNNEST
	bool RewriteCandidate(unique_ptr<LogicalOperator> &candidate);
	//! Update the bindings of the RHS sequence of LOGICAL_PROJECTION(s)
	void UpdateRHSBindings(unique_ptr<LogicalOperator> &plan, unique_ptr<LogicalOperator> &candidate,
	                       UnnestRewriterPlanUpdater &updater);
	//! Update the bindings of the BOUND_UNNEST expression of the LOGICAL_UNNEST
	void UpdateBoundUnnestBindings(UnnestRewriterPlanUpdater &updater, unique_ptr<LogicalOperator> &candidate);

	//! Store all delim columns of the delim join
	void GetDelimColumns(LogicalOperator &op);
	//! Store all LHS expressions of the LOGICAL_PROJECTION
	void GetLHSExpressions(LogicalOperator &op);

	//! Keep track of the delim columns to find the correct UNNEST column
	vector<ColumnBinding> delim_columns;
	//! Store the column bindings of the LHS child of the LOGICAL_DELIM_JOIN
	vector<LHSBinding> lhs_bindings;
	//! Stores the table index of the former child of the LOGICAL_UNNEST
	idx_t overwritten_tbl_idx;
	//! The number of distinct columns to unnest
	idx_t distinct_unnest_count;
};

} // namespace goose
