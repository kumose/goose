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

#include <goose/common/vector.h>
#include <goose/planner/column_binding_map.h>
#include <goose/planner/logical_operator_visitor.h>

namespace goose {

class Optimizer;
class BoundColumnRefExpression;

//! The ColumnLifetimeAnalyzer optimizer traverses the logical operator tree and ensures that columns are removed from
//! the plan when no longer required
class ColumnLifetimeAnalyzer : public LogicalOperatorVisitor {
public:
	explicit ColumnLifetimeAnalyzer(Optimizer &optimizer_p, LogicalOperator &root_p, bool is_root = false)
	    : optimizer(optimizer_p), root(root_p), everything_referenced(is_root) {
	}

	void VisitOperator(LogicalOperator &op) override;

public:
	static void ExtractColumnBindings(const Expression &expr, vector<ColumnBinding> &bindings);

protected:
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	unique_ptr<Expression> VisitReplace(BoundReferenceExpression &expr, unique_ptr<Expression> *expr_ptr) override;

private:
	Optimizer &optimizer;
	LogicalOperator &root;
	//! Whether or not all the columns are referenced. This happens in the case of the root expression (because the
	//! output implicitly refers all the columns below it)
	bool everything_referenced;
	//! The set of column references
	column_binding_set_t column_references;

private:
	void VisitOperatorInternal(LogicalOperator &op);
	void StandardVisitOperator(LogicalOperator &op);
	void ExtractUnusedColumnBindings(const vector<ColumnBinding> &bindings, column_binding_set_t &unused_bindings);
	static void GenerateProjectionMap(vector<ColumnBinding> bindings, column_binding_set_t &unused_bindings,
	                                  vector<idx_t> &map);
	void Verify(LogicalOperator &op);
	void AddVerificationProjection(unique_ptr<LogicalOperator> &child);
};
} // namespace goose
