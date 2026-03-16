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
