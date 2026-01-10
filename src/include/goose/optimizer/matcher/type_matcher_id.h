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

#include <goose/common/types.h>
#include <goose/optimizer/matcher/type_matcher.h>
namespace goose {

//! The TypeMatcherId class contains a set of matchers that can be used to pattern match TypeIds for Rules
class TypeMatcherId : public TypeMatcher {
public:
	explicit TypeMatcherId(LogicalTypeId type_id_p) : type_id(type_id_p) {
	}

	bool Match(const LogicalType &type) override {
		return type.id() == this->type_id;
	}

private:
	LogicalTypeId type_id;
};

} // namespace goose
