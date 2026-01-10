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
#include <goose/common/exception.h>

namespace goose {

enum class OrderType : uint8_t { INVALID = 0, ORDER_DEFAULT = 1, ASCENDING = 2, DESCENDING = 3 };

enum class OrderByNullType : uint8_t { INVALID = 0, ORDER_DEFAULT = 1, NULLS_FIRST = 2, NULLS_LAST = 3 };

enum class DefaultOrderByNullType : uint8_t {
	INVALID = 0,
	NULLS_FIRST = 2,
	NULLS_LAST = 3,
	NULLS_FIRST_ON_ASC_LAST_ON_DESC = 4,
	NULLS_LAST_ON_ASC_FIRST_ON_DESC = 5
};

} // namespace goose
