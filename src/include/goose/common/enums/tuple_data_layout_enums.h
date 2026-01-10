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

namespace goose {

enum class TupleDataValidityType : uint8_t {
	CAN_HAVE_NULL_VALUES = 0,
	CANNOT_HAVE_NULL_VALUES = 1,
};

enum class TupleDataNestednessType : uint8_t {
	TOP_LEVEL_LAYOUT = 0,
	NESTED_STRUCT_LAYOUT = 1,
};

} // namespace goose
