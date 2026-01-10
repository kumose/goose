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

//===--------------------------------------------------------------------===//
// Index Constraint Types
//===--------------------------------------------------------------------===//
enum class IndexConstraintType : uint8_t {
	NONE = 0,    // index is an index don't built to any constraint
	UNIQUE = 1,  // index is an index built to enforce a UNIQUE constraint
	PRIMARY = 2, // index is an index built to enforce a PRIMARY KEY constraint
	FOREIGN = 3  // index is an index built to enforce a FOREIGN KEY constraint
};

//===--------------------------------------------------------------------===//
// Index Types
//===--------------------------------------------------------------------===//
// NOTE: deprecated. Still necessary to read older goose files.
enum class DeprecatedIndexType : uint8_t {
	INVALID = 0,    // invalid index type
	ART = 1,        // Adaptive Radix Tree
	EXTENSION = 100 // Extension index
};

} // namespace goose
