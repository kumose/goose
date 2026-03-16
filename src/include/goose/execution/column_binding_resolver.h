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

#include <goose/planner/logical_operator_visitor.h>
#include <goose/planner/column_binding_map.h>
#include <goose/common/vector.h>

namespace goose {

//! The ColumnBindingResolver resolves ColumnBindings into base tables
//! (table_index, column_index) into physical indices into the DataChunks that
//! are used within the execution engine
class ColumnBindingResolver : public LogicalOperatorVisitor {
public:
	explicit ColumnBindingResolver(bool verify_only = false);

	void VisitOperator(LogicalOperator &op) override;
	static void Verify(LogicalOperator &op);

protected:
	vector<ColumnBinding> bindings;
	vector<LogicalType> types;
	bool verify_only;

	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	static unordered_set<idx_t> VerifyInternal(LogicalOperator &op);
};
} // namespace goose
