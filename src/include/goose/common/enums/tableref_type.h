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
// Table Reference Types
//===--------------------------------------------------------------------===//
enum class TableReferenceType : uint8_t {
	INVALID = 0,         // invalid table reference type
	BASE_TABLE = 1,      // base table reference
	SUBQUERY = 2,        // output of a subquery
	JOIN = 3,            // output of join
	TABLE_FUNCTION = 5,  // table producing function
	EXPRESSION_LIST = 6, // expression list
	CTE = 7,             // Recursive CTE
	EMPTY_FROM = 8,      // placeholder for empty FROM
	PIVOT = 9,           // pivot statement
	SHOW_REF = 10,       // SHOW statement
	COLUMN_DATA = 11,    // column data collection
	DELIM_GET = 12,      // Delim get ref
	BOUND_TABLE_REF = 13 // Bound table ref
};

} // namespace goose
