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
