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

class BoundForeignKeyConstraint : public BoundConstraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::FOREIGN_KEY;

public:
	BoundForeignKeyConstraint(ForeignKeyInfo info_p, physical_index_set_t pk_key_set_p,
	                          physical_index_set_t fk_key_set_p)
	    : BoundConstraint(ConstraintType::FOREIGN_KEY), info(std::move(info_p)), pk_key_set(std::move(pk_key_set_p)),
	      fk_key_set(std::move(fk_key_set_p)) {
#ifdef DEBUG
		D_ASSERT(info.pk_keys.size() == pk_key_set.size());
		for (auto &key : info.pk_keys) {
			D_ASSERT(pk_key_set.find(key) != pk_key_set.end());
		}
		D_ASSERT(info.fk_keys.size() == fk_key_set.size());
		for (auto &key : info.fk_keys) {
			D_ASSERT(fk_key_set.find(key) != fk_key_set.end());
		}
#endif
	}

	ForeignKeyInfo info;
	//! The same keys but stored as an unordered set
	physical_index_set_t pk_key_set;
	//! The same keys but stored as an unordered set
	physical_index_set_t fk_key_set;

	unique_ptr<BoundConstraint> Copy() const override {
		return make_uniq<BoundForeignKeyConstraint>(info, pk_key_set, fk_key_set);
	}
};

} // namespace goose
