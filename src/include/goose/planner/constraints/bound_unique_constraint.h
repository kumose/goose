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
#include <goose/common/index_map.h>

namespace goose {

class BoundUniqueConstraint : public BoundConstraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::UNIQUE;

public:
	BoundUniqueConstraint(vector<PhysicalIndex> keys_p, physical_index_set_t key_set_p, const bool is_primary_key)
	    : BoundConstraint(ConstraintType::UNIQUE), keys(std::move(keys_p)), key_set(std::move(key_set_p)),
	      is_primary_key(is_primary_key) {
#ifdef DEBUG
		D_ASSERT(keys.size() == key_set.size());
		for (auto &key : keys) {
			D_ASSERT(key_set.find(key) != key_set.end());
		}
#endif
	}

	//! The keys that define the unique constraint.
	vector<PhysicalIndex> keys;
	//! The same keys but stored as an unordered set.
	physical_index_set_t key_set;
	//! Whether this is a PRIMARY KEY constraint, or a UNIQUE constraint.
	bool is_primary_key;

public:
	unique_ptr<BoundConstraint> Copy() const override {
		return make_uniq<BoundUniqueConstraint>(keys, key_set, is_primary_key);
	}
};

} // namespace goose
