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

#include <goose/common/optional_ptr.h>
#include <goose/common/typedefs.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/planner/column_binding.h>
#include <goose/planner/column_binding_map.h>
#include <goose/planner/expression.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/logical_operator_visitor.h>

namespace goose {
class JoinElimination;
class PipelineInfo;

struct DistinctGroupRef {
	column_binding_set_t distinct_group;
	unordered_set<idx_t> ref_column_ids;
};

class PipelineInfo {
public:
	unique_ptr<LogicalOperator> root = nullptr;
	unordered_set<idx_t> ref_table_ids;
	unordered_map<idx_t, column_binding_set_t> distinct_groups;

	// pushdown filter condition(ex in table scan operator),
	// if have outer table columns then cannot elimination
	bool has_filter = false;

	optional_ptr<LogicalOperator> join_parent = nullptr;
	idx_t join_index = 0;

public:
	PipelineInfo CreateChild() {
		auto result = PipelineInfo();
		result.ref_table_ids = ref_table_ids;
		result.distinct_groups = distinct_groups;
		return result;
	}
};

class JoinElimination : public LogicalOperatorVisitor {
public:
	explicit JoinElimination() {
	}

	void OptimizeChildren(LogicalOperator &op, optional_ptr<LogicalOperator> parent, idx_t idx);
	// with specific condition we can eliminate a (left/right, semi, inner) join.
	// exemplify left/right join eliminaion condition:
	// 1. output can only have outer table columns
	// 2. join result cannot filter by inner table columns(ex. in where clause/ having clause ...)
	// 3. must ensure each outer row can match at most one inner table row, such as:
	//  1) inner table join condition is unique(ex. 1. join conditions have inner table's primary key 2. inner table
	//  join condition columns contains a whole distinct group) 2) join result columns contains a whole distinct group
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);
	void OptimizeInternal(unique_ptr<LogicalOperator> op);
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;

	unique_ptr<JoinElimination> CreateChildren() {
		auto result = make_uniq<JoinElimination>();
		result->pipe_info = pipe_info.CreateChild();
		return result;
	}

private:
	unique_ptr<LogicalOperator> TryEliminateJoin();
	// void ExtractDistinctReferences(vector<Expression> &expressions, idx_t target_table_index);
	bool ContainDistinctGroup(vector<ColumnBinding> &exprs);

	PipelineInfo pipe_info;

	optional_ptr<LogicalOperator> children_root;
	vector<unique_ptr<JoinElimination>> children;

	unique_ptr<JoinElimination> left_child;
	unique_ptr<JoinElimination> right_child;
};
} // namespace goose
