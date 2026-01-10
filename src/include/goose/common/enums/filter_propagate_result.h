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

enum class FilterPropagateResult : uint8_t {
	NO_PRUNING_POSSIBLE = 0,
	FILTER_ALWAYS_TRUE = 1,
	FILTER_ALWAYS_FALSE = 2,
	FILTER_TRUE_OR_NULL = 3,
	FILTER_FALSE_OR_NULL = 4
};

} // namespace goose
