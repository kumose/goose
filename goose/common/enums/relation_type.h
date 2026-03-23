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
// Catalog Types
//===--------------------------------------------------------------------===//
enum class RelationType : uint8_t {
	INVALID_RELATION,
	TABLE_RELATION,
	PROJECTION_RELATION,
	FILTER_RELATION,
	EXPLAIN_RELATION,
	CROSS_PRODUCT_RELATION,
	JOIN_RELATION,
	AGGREGATE_RELATION,
	SET_OPERATION_RELATION,
	DISTINCT_RELATION,
	LIMIT_RELATION,
	ORDER_RELATION,
	CREATE_VIEW_RELATION,
	CREATE_TABLE_RELATION,
	INSERT_RELATION,
	VALUE_LIST_RELATION,
	MATERIALIZED_RELATION,
	DELETE_RELATION,
	UPDATE_RELATION,
	WRITE_CSV_RELATION,
	WRITE_PARQUET_RELATION,
	READ_CSV_RELATION,
	SUBQUERY_RELATION,
	TABLE_FUNCTION_RELATION,
	VIEW_RELATION,
	QUERY_RELATION,
	DELIM_JOIN_RELATION,
	DELIM_GET_RELATION,
	EXTENSION_RELATION = 255
};

string RelationTypeToString(RelationType type);

} // namespace goose
