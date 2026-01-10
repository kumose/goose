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
