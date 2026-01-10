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

#include <goose/planner/bound_constraint.h>

namespace goose {

class BoundNotNullConstraint : public BoundConstraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::NOT_NULL;

public:
	explicit BoundNotNullConstraint(PhysicalIndex index) : BoundConstraint(ConstraintType::NOT_NULL), index(index) {
	}

	//! Column index this constraint pertains to
	PhysicalIndex index;

	unique_ptr<BoundConstraint> Copy() const override {
		return make_uniq<BoundNotNullConstraint>(index);
	}
};

} // namespace goose
