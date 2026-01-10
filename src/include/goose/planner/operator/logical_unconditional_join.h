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

namespace goose {

//! LogicalUnconditionalJoin represents a join between two relations
//! where the join condition is implicit (cross product, position, etc.)
class LogicalUnconditionalJoin : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	explicit LogicalUnconditionalJoin(LogicalOperatorType logical_type) : LogicalOperator(logical_type) {};

public:
	LogicalUnconditionalJoin(LogicalOperatorType logical_type, unique_ptr<LogicalOperator> left,
	                         unique_ptr<LogicalOperator> right);

public:
	vector<ColumnBinding> GetColumnBindings() override;

protected:
	void ResolveTypes() override;
};
} // namespace goose
