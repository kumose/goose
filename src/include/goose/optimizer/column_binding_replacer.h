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

#include <goose/planner/logical_operator.h>

namespace goose {

struct ReplacementBinding {
public:
	ReplacementBinding(ColumnBinding old_binding, ColumnBinding new_binding);
	ReplacementBinding(ColumnBinding old_binding, ColumnBinding new_binding, LogicalType new_type);

public:
	ColumnBinding old_binding;
	ColumnBinding new_binding;

	bool replace_type;
	LogicalType new_type;
};

//! The ColumnBindingReplacer updates column bindings (e.g., after changing the operator plan), utility for optimizers
class ColumnBindingReplacer : LogicalOperatorVisitor {
public:
	ColumnBindingReplacer();

public:
	//! Update each operator of the plan
	void VisitOperator(LogicalOperator &op) override;
	//! Visit an expression and update its column bindings
	void VisitExpression(unique_ptr<Expression> *expression) override;

public:
	//! Contains all bindings that need to be updated
	vector<ReplacementBinding> replacement_bindings;

	//! Do not recurse further than this operator (optional)
	optional_ptr<LogicalOperator> stop_operator;
};

} // namespace goose
