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
#include <goose/common/vector.h>

namespace goose {

enum class OptimizerType : uint32_t {
	INVALID = 0,
	EXPRESSION_REWRITER,
	FILTER_PULLUP,
	FILTER_PUSHDOWN,
	EMPTY_RESULT_PULLUP,
	CTE_FILTER_PUSHER,
	REGEX_RANGE,
	IN_CLAUSE,
	JOIN_ORDER,
	DELIMINATOR,
	UNNEST_REWRITER,
	UNUSED_COLUMNS,
	STATISTICS_PROPAGATION,
	COMMON_SUBEXPRESSIONS,
	COMMON_AGGREGATE,
	COLUMN_LIFETIME,
	BUILD_SIDE_PROBE_SIDE,
	LIMIT_PUSHDOWN,
	ROW_GROUP_PRUNER,
	TOP_N,
	TOP_N_WINDOW_ELIMINATION,
	COMPRESSED_MATERIALIZATION,
	DUPLICATE_GROUPS,
	REORDER_FILTER,
	SAMPLING_PUSHDOWN,
	JOIN_FILTER_PUSHDOWN,
	EXTENSION,
	MATERIALIZED_CTE,
	SUM_REWRITER,
	LATE_MATERIALIZATION,
	CTE_INLINING,
	COMMON_SUBPLAN,
	JOIN_ELIMINATION,
	WINDOW_SELF_JOIN
};

string OptimizerTypeToString(OptimizerType type);
OptimizerType OptimizerTypeFromString(const string &str);
vector<string> ListAllOptimizers();

} // namespace goose
