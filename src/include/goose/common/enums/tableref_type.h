// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
