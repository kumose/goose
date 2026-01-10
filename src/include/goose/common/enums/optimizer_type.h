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
