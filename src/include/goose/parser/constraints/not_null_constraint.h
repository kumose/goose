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

namespace goose {

class NotNullConstraint : public Constraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::NOT_NULL;

public:
	GOOSE_API explicit NotNullConstraint(LogicalIndex index);
	GOOSE_API ~NotNullConstraint() override;

	//! Column index this constraint pertains to
	LogicalIndex index;

public:
	GOOSE_API string ToString() const override;

	GOOSE_API unique_ptr<Constraint> Copy() const override;

	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<Constraint> Deserialize(Deserializer &deserializer);
};

} // namespace goose
