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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/common/exception.h>

namespace goose {

class Serializer;
class Deserializer;

//===--------------------------------------------------------------------===//
// Constraint Types
//===--------------------------------------------------------------------===//
enum class ConstraintType : uint8_t {
	INVALID = 0,     // invalid constraint type
	NOT_NULL = 1,    // NOT NULL constraint
	CHECK = 2,       // CHECK constraint
	UNIQUE = 3,      // UNIQUE constraint
	FOREIGN_KEY = 4, // FOREIGN KEY constraint
};

enum class ForeignKeyType : uint8_t {
	FK_TYPE_PRIMARY_KEY_TABLE = 0,   // main table
	FK_TYPE_FOREIGN_KEY_TABLE = 1,   // referencing table
	FK_TYPE_SELF_REFERENCE_TABLE = 2 // self refrencing table
};

struct ForeignKeyInfo {
	ForeignKeyType type;
	string schema;
	//! if type is FK_TYPE_FOREIGN_KEY_TABLE, means main key table, if type is FK_TYPE_PRIMARY_KEY_TABLE, means foreign
	//! key table
	string table;
	//! The set of main key table's column's index
	vector<PhysicalIndex> pk_keys;
	//! The set of foreign key table's column's index
	vector<PhysicalIndex> fk_keys;

	bool IsDeleteConstraint() const {
		return type == ForeignKeyType::FK_TYPE_PRIMARY_KEY_TABLE ||
		       type == ForeignKeyType::FK_TYPE_SELF_REFERENCE_TABLE;
	}
	bool IsAppendConstraint() const {
		return type == ForeignKeyType::FK_TYPE_FOREIGN_KEY_TABLE ||
		       type == ForeignKeyType::FK_TYPE_SELF_REFERENCE_TABLE;
	}
};

//! Constraint is the base class of any type of table constraint.
class Constraint {
public:
	GOOSE_API explicit Constraint(ConstraintType type);
	GOOSE_API virtual ~Constraint();

	ConstraintType type;

public:
	GOOSE_API virtual string ToString() const = 0;
	GOOSE_API void Print() const;

	GOOSE_API virtual unique_ptr<Constraint> Copy() const = 0;

	GOOSE_API virtual void Serialize(Serializer &serializer) const;
	GOOSE_API static unique_ptr<Constraint> Deserialize(Deserializer &deserializer);

public:
	template <class TARGET>
	TARGET &Cast() {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast constraint to type - constraint type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast constraint to type - constraint type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}
};
} // namespace goose
