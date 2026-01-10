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

#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_get.h>

namespace goose {

struct LateMaterializationHelper {
	static unique_ptr<LogicalGet> CreateLHSGet(const LogicalGet &rhs, Binder &binder);
	static vector<idx_t> GetOrInsertRowIds(LogicalGet &get, const vector<column_t> &row_id_column_ids,
	                                       const vector<TableColumn> &row_id_columns);
};

} // namespace goose
