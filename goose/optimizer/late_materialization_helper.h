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

#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_get.h>

namespace goose {

struct LateMaterializationHelper {
	static unique_ptr<LogicalGet> CreateLHSGet(const LogicalGet &rhs, Binder &binder);
	static vector<idx_t> GetOrInsertRowIds(LogicalGet &get, const vector<column_t> &row_id_column_ids,
	                                       const vector<TableColumn> &row_id_columns);
};

} // namespace goose
