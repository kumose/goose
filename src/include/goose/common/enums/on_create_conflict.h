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

enum class OnCreateConflict : uint8_t {
	// Standard: throw error
	ERROR_ON_CONFLICT,
	// CREATE IF NOT EXISTS, silently do nothing on conflict
	IGNORE_ON_CONFLICT,
	// CREATE OR REPLACE
	REPLACE_ON_CONFLICT,
	// Update on conflict - only support for functions. Add a function overload if the function already exists.
	ALTER_ON_CONFLICT
};

} // namespace goose
