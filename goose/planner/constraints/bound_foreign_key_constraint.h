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
