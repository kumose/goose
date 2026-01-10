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
