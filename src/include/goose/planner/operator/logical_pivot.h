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

#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/planner/logical_operator.h>
#include <goose/parser/tableref/pivotref.h>
#include <goose/planner/tableref/bound_pivotref.h>

namespace goose {

class LogicalPivot : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_PIVOT;

public:
	LogicalPivot(idx_t pivot_idx, unique_ptr<LogicalOperator> plan, BoundPivotInfo info);

	idx_t pivot_index;
	//! The bound pivot info
	BoundPivotInfo bound_pivot;

public:
	vector<ColumnBinding> GetColumnBindings() override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override;

private:
	LogicalPivot();
};
} // namespace goose
