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
#include <goose/planner/bound_constraint.h>
#include <goose/planner/expression.h>
#include <goose/common/index_map.h>

namespace goose {

//! The CheckConstraint contains an expression that must evaluate to TRUE for
//! every row in a table
class BoundCheckConstraint : public BoundConstraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::CHECK;

public:
	BoundCheckConstraint() : BoundConstraint(ConstraintType::CHECK) {
	}

	//! The expression
	unique_ptr<Expression> expression;
	//! The columns used by the CHECK constraint
	physical_index_set_t bound_columns;

public:
	unique_ptr<BoundConstraint> Copy() const override {
		auto result = make_uniq<BoundCheckConstraint>();
		result->expression = expression->Copy();
		result->bound_columns = bound_columns;
		return std::move(result);
	}
};

} // namespace goose
