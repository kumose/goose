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

#include <goose/parser/constraint.h>
#include <goose/common/vector.h>

namespace goose {

class ForeignKeyConstraint : public Constraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::FOREIGN_KEY;

public:
	GOOSE_API ForeignKeyConstraint(vector<string> pk_columns, vector<string> fk_columns, ForeignKeyInfo info);

	//! The set of main key table's columns
	vector<string> pk_columns;
	//! The set of foreign key table's columns
	vector<string> fk_columns;
	ForeignKeyInfo info;

public:
	GOOSE_API string ToString() const override;

	GOOSE_API unique_ptr<Constraint> Copy() const override;

	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<Constraint> Deserialize(Deserializer &deserializer);

private:
	ForeignKeyConstraint();
};

} // namespace goose
