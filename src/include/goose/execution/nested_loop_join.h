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

#include <goose/common/common.h>
#include <goose/common/types/column/column_data_collection.h>
#include <goose/common/types/vector.h>
#include <goose/planner/operator/logical_comparison_join.h>

namespace goose {
class ColumnDataCollection;

struct NestedLoopJoinInner {
	static idx_t Perform(idx_t &ltuple, idx_t &rtuple, DataChunk &left_conditions, DataChunk &right_conditions,
	                     SelectionVector &lvector, SelectionVector &rvector, const vector<JoinCondition> &conditions);
};

struct NestedLoopJoinMark {
	static void Perform(DataChunk &left, ColumnDataCollection &right, bool found_match[],
	                    const vector<JoinCondition> &conditions);
};

} // namespace goose
