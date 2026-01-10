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
// Physical Operator Types
//===--------------------------------------------------------------------===//
enum class PhysicalOperatorType : uint8_t {
	INVALID,
	ORDER_BY,
	LIMIT,
	STREAMING_LIMIT,
	LIMIT_PERCENT,
	TOP_N,
	WINDOW,
	UNNEST,
	UNGROUPED_AGGREGATE,
	HASH_GROUP_BY,
	PERFECT_HASH_GROUP_BY,
	PARTITIONED_AGGREGATE,
	FILTER,
	PROJECTION,
	COPY_TO_FILE,
	BATCH_COPY_TO_FILE,
	RESERVOIR_SAMPLE,
	STREAMING_SAMPLE,
	STREAMING_WINDOW,
	PIVOT,
	COPY_DATABASE,

	// -----------------------------
	// Scans
	// -----------------------------
	TABLE_SCAN,
	DUMMY_SCAN,
	COLUMN_DATA_SCAN,
	CHUNK_SCAN,
	RECURSIVE_CTE_SCAN,
	RECURSIVE_RECURRING_CTE_SCAN,
	CTE_SCAN,
	DELIM_SCAN,
	EXPRESSION_SCAN,
	POSITIONAL_SCAN,
	// -----------------------------
	// Joins
	// -----------------------------
	BLOCKWISE_NL_JOIN,
	NESTED_LOOP_JOIN,
	HASH_JOIN,
	CROSS_PRODUCT,
	PIECEWISE_MERGE_JOIN,
	IE_JOIN,
	LEFT_DELIM_JOIN,
	RIGHT_DELIM_JOIN,
	POSITIONAL_JOIN,
	ASOF_JOIN,
	// -----------------------------
	// SetOps
	// -----------------------------
	UNION,
	RECURSIVE_CTE,
	RECURSIVE_KEY_CTE,
	CTE,

	// -----------------------------
	// Updates
	// -----------------------------
	INSERT,
	BATCH_INSERT,
	DELETE_OPERATOR,
	UPDATE,
	MERGE_INTO,

	// -----------------------------
	// Schema
	// -----------------------------
	CREATE_TABLE,
	CREATE_TABLE_AS,
	BATCH_CREATE_TABLE_AS,
	CREATE_INDEX,
	ALTER,
	CREATE_SEQUENCE,
	CREATE_VIEW,
	CREATE_SCHEMA,
	CREATE_MACRO,
	DROP,
	PRAGMA,
	TRANSACTION,
	CREATE_TYPE,
	ATTACH,
	DETACH,

	// -----------------------------
	// Helpers
	// -----------------------------
	EXPLAIN,
	EXPLAIN_ANALYZE,
	EMPTY_RESULT,
	EXECUTE,
	PREPARE,
	VACUUM,
	EXPORT,
	SET,
	SET_VARIABLE,
	LOAD,
	INOUT_FUNCTION,
	RESULT_COLLECTOR,
	RESET,
	EXTENSION,
	VERIFY_VECTOR,
	UPDATE_EXTENSIONS,

	// -----------------------------
	// Secret
	// -----------------------------
	CREATE_SECRET,
};

string PhysicalOperatorToString(PhysicalOperatorType type);

} // namespace goose
