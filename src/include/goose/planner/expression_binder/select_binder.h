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

#include <goose/planner/expression_binder/base_select_binder.h>

namespace goose {

//! The SELECT binder is responsible for binding an expression within the SELECT clause of a SQL statement
class SelectBinder : public BaseSelectBinder {
public:
	SelectBinder(Binder &binder, ClientContext &context, BoundSelectNode &node, BoundGroupInformation &info);

	bool TryResolveAliasReference(ColumnRefExpression &colref, idx_t depth, bool root_expression, BindResult &result,
	                              unique_ptr<ParsedExpression> &expr_ptr) override;
	bool DoesColumnAliasExist(const ColumnRefExpression &colref) override;

protected:
	void ThrowIfUnnestInLambda(const ColumnBinding &column_binding) override;
	BindResult BindUnnest(FunctionExpression &function, idx_t depth, bool root_expression) override;

	unique_ptr<ParsedExpression> GetSQLValueFunction(const string &column_name) override;

protected:
	idx_t unnest_level = 0;
};

} // namespace goose
