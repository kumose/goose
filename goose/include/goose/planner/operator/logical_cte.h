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

#include <goose/planner/logical_operator.h>
#include <goose/planner/binder.h>

namespace goose {

class LogicalCTE : public LogicalOperator {
public:
	explicit LogicalCTE(LogicalOperatorType logical_type = LogicalOperatorType::LOGICAL_INVALID)
	    : LogicalOperator(logical_type) {
	}

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	explicit LogicalCTE(string ctename_p, idx_t table_index, idx_t column_count, unique_ptr<LogicalOperator> top,
	                    unique_ptr<LogicalOperator> bottom,
	                    LogicalOperatorType logical_type = LogicalOperatorType::LOGICAL_INVALID)
	    : LogicalOperator(logical_type), ctename(std::move(ctename_p)), table_index(table_index),
	      column_count(column_count) {
		children.push_back(std::move(top));
		children.push_back(std::move(bottom));
	}

	string ctename;
	idx_t table_index;
	idx_t column_count;
	CorrelatedColumns correlated_columns;
};
} // namespace goose
